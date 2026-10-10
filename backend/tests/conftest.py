"""Ensure the package is importable when running pytest without installation."""

import os
import sys

import pytest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


# anyio pytest plugin needs an explicit backend fixture for @pytest.mark.anyio.
# Prefer asyncio (always available); trio is not installed here.
@pytest.fixture
def anyio_backend():
    return "asyncio"
