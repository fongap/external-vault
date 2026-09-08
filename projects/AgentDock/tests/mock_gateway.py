import json
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer


class GatewayHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def _reply(self, status, payload):
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Connection", "close")
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if self.path != "/v1/models":
            self._reply(404, {"error": "not found"})
            return
        expected = "test-key"
        if not (
            self.headers.get("Authorization") == f"Bearer {expected}"
            and self.headers.get("x-api-key") == expected
            and self.headers.get("x-gateway-access-key") == expected
        ):
            self._reply(401, {"error": "v1.0-compatible headers required"})
            return
        self._reply(200, {"data": [{"id": "claude-sonnet-test", "context_window": 200000}]})

    def do_POST(self):
        length = int(self.headers.get("Content-Length", "0"))
        try:
            payload = json.loads(self.rfile.read(length))
        except (ValueError, json.JSONDecodeError):
            self._reply(400, {"error": "invalid json"})
            return
        if self.path != "/v1/messages" or payload.get("model") != "claude-sonnet-test":
            self._reply(400, {"error": "real model id required"})
            return
        auth_headers = [
            self.headers.get("Authorization"),
            self.headers.get("x-api-key"),
            self.headers.get("x-gateway-access-key"),
        ]
        if auth_headers == [None, "test-key", None]:
            self._reply(422, {"error": "route and authentication accepted"})
        else:
            self._reply(401, {"error": "use x-api-key only"})

    def log_message(self, _format, *_args):
        pass


if __name__ == "__main__":
    ThreadingHTTPServer(("127.0.0.1", 18765), GatewayHandler).serve_forever()
