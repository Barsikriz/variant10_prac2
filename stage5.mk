PYTHON ?= python

.PHONY: test-stage5 demo compare

test-stage5: test-stage4
	powershell -ExecutionPolicy Bypass -File tests/stage5.ps1

demo: depgraph.exe
	$(PYTHON) tools/demo5.py

compare: depgraph.exe
	$(PYTHON) tools/demo5.py --compare
