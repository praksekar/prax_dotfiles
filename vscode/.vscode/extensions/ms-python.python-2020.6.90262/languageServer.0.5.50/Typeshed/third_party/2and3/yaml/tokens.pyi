from typing import Any

class Token:
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    def __init__(self, start_mark, end_mark) -> None: ...

class DirectiveToken(Token):
    id = ...  # type: Any
    name = ...  # type: Any
    value = ...  # type: Any
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    def __init__(self, name, value, start_mark, end_mark) -> None: ...

class DocumentStartToken(Token):
    id = ...  # type: Any

class DocumentEndToken(Token):
    id = ...  # type: Any

class StreamStartToken(Token):
    id = ...  # type: Any
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    encoding = ...  # type: Any
    def __init__(self, start_mark=..., end_mark=..., encoding=...) -> None: ...

class StreamEndToken(Token):
    id = ...  # type: Any

class BlockSequenceStartToken(Token):
    id = ...  # type: Any

class BlockMappingStartToken(Token):
    id = ...  # type: Any

class BlockEndToken(Token):
    id = ...  # type: Any

class FlowSequenceStartToken(Token):
    id = ...  # type: Any

class FlowMappingStartToken(Token):
    id = ...  # type: Any

class FlowSequenceEndToken(Token):
    id = ...  # type: Any

class FlowMappingEndToken(Token):
    id = ...  # type: Any

class KeyToken(Token):
    id = ...  # type: Any

class ValueToken(Token):
    id = ...  # type: Any

class BlockEntryToken(Token):
    id = ...  # type: Any

class FlowEntryToken(Token):
    id = ...  # type: Any

class AliasToken(Token):
    id = ...  # type: Any
    value = ...  # type: Any
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    def __init__(self, value, start_mark, end_mark) -> None: ...

class AnchorToken(Token):
    id = ...  # type: Any
    value = ...  # type: Any
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    def __init__(self, value, start_mark, end_mark) -> None: ...

class TagToken(Token):
    id = ...  # type: Any
    value = ...  # type: Any
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    def __init__(self, value, start_mark, end_mark) -> None: ...

class ScalarToken(Token):
    id = ...  # type: Any
    value = ...  # type: Any
    plain = ...  # type: Any
    start_mark = ...  # type: Any
    end_mark = ...  # type: Any
    style = ...  # type: Any
    def __init__(self, value, plain, start_mark, end_mark, style=...) -> None: ...
