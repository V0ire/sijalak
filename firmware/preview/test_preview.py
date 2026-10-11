import argparse
import json
import threading
import unittest
from http.server import HTTPServer
from urllib.error import HTTPError
from urllib.request import Request, urlopen

from serve import Engine, handler

RUNNER = None


class PreviewTests(unittest.TestCase):
    def setUp(self):
        self.engine = Engine(RUNNER)

    def tearDown(self):
        self.engine.close()

    def test_compiled_gestures_timer_frames(self):
        state = self.engine.request("state", True)
        self.assertEqual(state["screen"], "face")
        self.assertEqual(len(state["pages"]), 1024)
        self.assertTrue(all(isinstance(b, int) and 0 <= b <= 255 for b in state["pages"]))
        self.assertEqual(self.engine.request("tap")["screen"], "menu")
        self.assertEqual(self.engine.request("tap")["selection"], 1)
        self.assertEqual(self.engine.request("double")["screen"], "timer")
        self.assertTrue(self.engine.request("double")["running"])
        paused = self.engine.request("double")
        self.assertFalse(paused["running"])
        self.assertEqual(self.engine.request("focus-end")["remaining_ms"], paused["remaining_ms"])
        self.engine.request("double")
        self.assertEqual(self.engine.request("focus-end")["phase"], "break")
        self.assertEqual(self.engine.request("break-end")["phase"], "done")
        self.assertTrue(self.engine.request("double")["running"])
        reset = self.engine.request("hold")
        self.assertEqual(reset["screen"], "face")
        self.assertFalse(reset["running"])
        self.assertEqual(reset["remaining_ms"], 1_500_000)

    def test_reduced_motion_and_invalid_inputs(self):
        self.assertEqual(self.engine.request("state", True)["pages"], self.engine.request("focus-end", True)["pages"])
        with self.assertRaises(ValueError):
            self.engine.request("audio")
        with self.assertRaises(ValueError):
            self.engine.sample(2**64, False)

    def test_loopback_http_boundaries(self):
        # Ephemeral loopback port; no external network/provider requests.
        with HTTPServer(("127.0.0.1", 0), handler(self.engine, 0)) as server:
            port = server.server_address[1]
            server.RequestHandlerClass = handler(self.engine, port)
            thread = threading.Thread(target=server.serve_forever)
            thread.start()
            try:
                base = f"http://127.0.0.1:{port}"
                with urlopen(base, timeout=2) as response:
                    self.assertIn(b"runner", response.read())
                request = Request(base + "/input", data=b'{"action":"tap"}', method="POST")
                with urlopen(request, timeout=2) as response:
                    self.assertEqual(json.load(response)["screen"], "menu")
                for payload in (b'{}', b'{"action":"audio"}', b'{"action":"state","reduced":1}', b'x' * 1025):
                    with self.assertRaises(HTTPError) as error:
                        urlopen(Request(base + "/input", data=payload), timeout=2)
                    self.assertEqual(error.exception.code, 400)
                with self.assertRaises(HTTPError) as error:
                    urlopen(Request(base + "/input", data=b'{"action":"tap"}', headers={"Origin":"https://example.invalid"}), timeout=2)
                self.assertEqual(error.exception.code, 403)
            finally:
                server.shutdown()
                thread.join(timeout=2)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--runner", required=True)
    args = parser.parse_args()
    RUNNER = args.runner
    unittest.main(argv=[__file__])
