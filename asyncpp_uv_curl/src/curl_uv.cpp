/*
 * Based on https://curl.se/libcurl/c/multi-uv.html
 */

#include "asyncpp_uv_curl/curl_uv.h"

#include <curl/curl.h>
#include <uv.h>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <sstream>

namespace curl_uv {
CURLM* curlMultiHandle;
uv_timer_t uvTimeout;

void fillResponse(CurlSession* session, CURLcode result);

typedef struct curl_context_s {
  uv_poll_t poll_handle;
  curl_socket_t sockfd;
} curl_context_t;

static curl_context_t* curl_createContext(curl_socket_t sockfd) {
  curl_context_t* context;

  context = new curl_context_t();

  context->sockfd = sockfd;

  uv_loop_t* loop = uv_default_loop();
  uv_poll_init_socket(loop, &context->poll_handle, sockfd);
  context->poll_handle.data = context;

  return context;
}

static void curl_destroyContext(curl_context_t* context) {
  uv_close((uv_handle_t*)&context->poll_handle, [](uv_handle_t* handle) { delete (curl_context_t*)handle->data; });
}

static void curl_checkMultiInfo() {
  CURLMsg* message;
  int pending;

  while ((message = curl_multi_info_read(curlMultiHandle, &pending))) {
    switch (message->msg) {
      case CURLMSG_DONE: {
        /* Do not use message data after calling
                 curl_multi_remove_handle() and curl_easy_cleanup(). As
           per curl_multi_info_read() docs: "WARNING: The data the
           returned pointer points to will not survive calling
                 curl_multi_cleanup, curl_multi_remove_handle or
                                         curl_easy_cleanup." */
        CurlSession* session;
        curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, &session);

        CURLcode result = message->data.result;

        fillResponse(session, result);

        session->completedCb(std::move(session->response));

        curl_multi_remove_handle(curlMultiHandle, session->handle);
        curl_easy_cleanup(session->handle);
        delete session;
        break;
      }
      default:
        throw std::runtime_error("invalid curl message");
    }
  }
}

static void curl_perform(uv_poll_t* req, int status [[maybe_unused]], int events) {
  int running_handles;
  int flags = 0;
  curl_context_t* context;

  if (events & UV_READABLE) flags |= CURL_CSELECT_IN;
  if (events & UV_WRITABLE) flags |= CURL_CSELECT_OUT;

  context = (curl_context_t*)req->data;

  curl_multi_socket_action(curlMultiHandle, context->sockfd, flags, &running_handles);

  curl_checkMultiInfo();
}

static void curl_onTimeout(uv_timer_t* req [[maybe_unused]]) {
  int running_handles;
  curl_multi_socket_action(curlMultiHandle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
  curl_checkMultiInfo();
}

static int curl_startTimeout(CURLM* multi [[maybe_unused]], long timeout_ms, void* userp [[maybe_unused]]) {
  if (timeout_ms < 0) {
    uv_timer_stop(&uvTimeout);
  } else {
    if (timeout_ms == 0) timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it in a bit */
    uv_timer_start(&uvTimeout, curl_onTimeout, (unsigned long)timeout_ms, 0);
  }
  return 0;
}

static int curl_handleSocket(CURL* easy [[maybe_unused]], curl_socket_t s, int action, void* userp [[maybe_unused]],
                             void* socketp) {
  curl_context_t* curl_context;
  int events = 0;

  switch (action) {
    case CURL_POLL_IN:
    case CURL_POLL_OUT:
    case CURL_POLL_INOUT:
      curl_context = socketp ? (curl_context_t*)socketp : curl_createContext(s);

      curl_multi_assign(curlMultiHandle, s, (void*)curl_context);

      if (action != CURL_POLL_IN) events |= UV_WRITABLE;
      if (action != CURL_POLL_OUT) events |= UV_READABLE;

      uv_poll_start(&curl_context->poll_handle, events, curl_perform);
      break;
    case CURL_POLL_REMOVE:
      if (socketp) {
        uv_poll_stop(&((curl_context_t*)socketp)->poll_handle);
        curl_destroyContext((curl_context_t*)socketp);
        curl_multi_assign(curlMultiHandle, s, NULL);
      }
      break;
    default:
      abort();
  }

  return 0;
}

static size_t curl_writeFunction(void* data, size_t size, size_t nmemb, void* userp) {
  CurlSession* session = (CurlSession*)userp;

  size_t realsize = size * nmemb;

  std::vector<uint8_t>& buffer = session->response.buffer;

  size_t newSize = buffer.size() + realsize;

  while (newSize > buffer.capacity()) {
    buffer.reserve(buffer.capacity() * 2);
  }

  uint8_t* _data = (uint8_t*)data;
  buffer.insert(buffer.end(), _data, _data + realsize);

  return realsize;
}

CurlRequest get(const std::string& url) { return {Method::GET, url}; }
CurlRequest post(const std::string& url) { return {Method::POST, url}; }
CurlRequest delete_(const std::string& url) { return {Method::DELETE, url}; }

void fillRequest(CURL* curl, const CurlRequest& request) {
  curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, (long)request.connectTimeout.count());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, (long)request.timeout.count());
  if (request.slist != nullptr) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request.slist);

  if (request.verbose) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  switch (request.method) {
    default:
    case Method::GET:
      break;
    case Method::POST:
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.payload.data());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.payload.size());
      break;
    case Method::DELETE:
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.payload.data());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.payload.size());
      break;
  }
}

void fillResponse(CurlSession* session, CURLcode result) {
  long statusCode = 0;
  curl_easy_getinfo(session->handle, CURLINFO_RESPONSE_CODE, &statusCode);

  session->response.result = result;
  session->response.statusCode = statusCode;

  if (result != CURLE_OK) session->response.error = session->errorBuffer;
}

void execute(CurlRequest&& request, CurlCompletedCb completedCb) {
  CurlSession* session = new CurlSession();

  session->request = std::move(request);
  session->response.buffer.reserve(1024);
  session->handle = curl_easy_init();
  session->completedCb = std::move(completedCb);

  curl_easy_setopt(session->handle, CURLOPT_WRITEFUNCTION, curl_writeFunction);
  curl_easy_setopt(session->handle, CURLOPT_WRITEDATA, session);
  curl_easy_setopt(session->handle, CURLOPT_PRIVATE, session);
  curl_easy_setopt(session->handle, CURLOPT_ERRORBUFFER, session->errorBuffer);
  curl_easy_setopt(session->handle, CURLOPT_FOLLOWLOCATION, 1L);

  fillRequest(session->handle, session->request);

  CURLMcode c = curl_multi_add_handle(curlMultiHandle, session->handle);
  if (c != CURLM_OK) {
    delete session;
    throw std::runtime_error("curl add handle error");
  }
}

CurlResponse executeSync(CurlRequest&& request) {
  CurlSession* session = new CurlSession();

  session->request = std::move(request);
  session->response.buffer.reserve(1024);
  session->handle = curl_easy_init();

  curl_easy_setopt(session->handle, CURLOPT_WRITEFUNCTION, curl_writeFunction);
  curl_easy_setopt(session->handle, CURLOPT_WRITEDATA, session);
  curl_easy_setopt(session->handle, CURLOPT_PRIVATE, session);
  curl_easy_setopt(session->handle, CURLOPT_ERRORBUFFER, session->errorBuffer);
  curl_easy_setopt(session->handle, CURLOPT_FOLLOWLOCATION, 1L);

  fillRequest(session->handle, session->request);

  CURLcode result = curl_easy_perform(session->handle);

  fillResponse(session, result);

  curl_easy_cleanup(session->handle);

  CurlResponse response = std::move(session->response);
  delete session;

  return std::move(response);
}

void CurlRequest::addHeader(const std::string_view& name, const std::string_view& value) {
  std::stringstream ss;
  ss << name << ":" << value;
  slist = curl_slist_append(slist, ss.str().c_str());
}

class CurlInitializer {
  static const int init;
};

const int CurlInitializer::init = [] {
  curl_global_init(CURL_GLOBAL_ALL);

  uv_timer_init(uv_default_loop(), &uvTimeout);

  curlMultiHandle = curl_multi_init();
  curl_multi_setopt(curlMultiHandle, CURLMOPT_SOCKETFUNCTION, curl_handleSocket);
  curl_multi_setopt(curlMultiHandle, CURLMOPT_TIMERFUNCTION, curl_startTimeout);

  return 0;
}();

void unload() {
  curl_multi_cleanup(curlMultiHandle);
  uv_close((uv_handle_t*)&uvTimeout, nullptr);
  curl_global_cleanup();
}
}  // namespace curl_uv