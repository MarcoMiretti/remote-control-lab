from esplab import Resourcer
from socket import timeout
import json
import unittest

# TODO: this should be more automatic
AEROPENDULUM_IP_ADD = "192.168.100.41"


class HTTPTestCase(unittest.TestCase):
    def setUp(self):
        retries = 3
        while retries >= 0:
            try:
                self.resourcer = Resourcer(AEROPENDULUM_IP_ADD, port=80, timeout=20)
                break
            except timeout:
                print("Timed out, retrying...")
                retries -= 1

    def _test_resources(self, resource, value):
        RESOURCE = resource
        VALUE = value
        value_type = type(value)
        response = self.resourcer.post(RESOURCE, VALUE)
        self.assertEqual(response, 202)

        response = self.resourcer.get(RESOURCE)
        self.assertEqual(value_type(response), VALUE)


    def test_dummy_resource(self):
        RESOURCE = "/test/resource"
        VALUE = "hithere"
        self._test_resources(RESOURCE, VALUE)

    def test_logger(self):
        RESOURCE = "/logger/level"
        LOG_LEVEL = "LOG_TRACE"
        self._test_resources(RESOURCE, LOG_LEVEL)

        LOG_LEVEL = "LOG_INFO"
        self._test_resources(RESOURCE, LOG_LEVEL)

    def test_telemetry_period(self):
        RESOURCE = "/telemetry/period"
        PERIOD = 20
        self._test_resources(RESOURCE, PERIOD)

        DEFAULT_PERIOD = 500
        self._test_resources(RESOURCE, DEFAULT_PERIOD)

    def test_propeller_pwm_init(self):
        RESOURCE = "/propeller/pwm/status"
        VALUE = "disabled"
        self._test_resources(RESOURCE, VALUE)

        VALUE = "initialized"
        self._test_resources(RESOURCE, VALUE)

    def test_child_resources(self):
        RESOURCE = "/test/parent_resource/child_a"
        VALUE_A = "0xFAFA"
        self._test_resources(RESOURCE, VALUE_A)

        RESOURCE = "/test/parent_resource/child_b"
        VALUE_B = "0x0EFE"
        self._test_resources(RESOURCE, VALUE_B)


        RESOURCE = "/test/parent_resource"
        response = self.resourcer.get(RESOURCE)

        json_data = {"child_a": VALUE_A, "child_b": VALUE_B}
        expected = json.dumps(json_data)

        self.assertEqual(response, expected)
