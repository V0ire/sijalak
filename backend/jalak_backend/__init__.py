"""Offline-testable backend core for Si Jalak Bedas.

Everything here is provider- and framework-agnostic so it can run with no
credentials, no network, and no hardware. Real provider adapters and the HTTP
layer are deliberately out of scope for this module.
"""

from .limits import Limits, Admission, admit_turn, admit_status_poll
from .sessions import SessionStore, SessionError
from .reply_schema import Reply, ReplyValidationError, validate_reply
from .guardrails import inspect_user_text, GuardrailResult
from .provider import StubSpeechProvider, SpeechResult
from .cost import CostAdmission, CostPolicy
from .gateway import GatewayConfig, GatewayService, create_app
from .wire import TurnWire, SCHEMA_VERSION

__all__ = [
    "Limits",
    "Admission",
    "admit_turn",
    "admit_status_poll",
    "SessionStore",
    "SessionError",
    "Reply",
    "ReplyValidationError",
    "validate_reply",
    "inspect_user_text",
    "GuardrailResult",
    "StubSpeechProvider",
    "SpeechResult",
    "CostAdmission",
    "CostPolicy",
    "GatewayConfig",
    "GatewayService",
    "create_app",
    "TurnWire",
    "SCHEMA_VERSION",
]
