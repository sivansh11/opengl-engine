
import engine_native as en
# import imgui

def register_class(cls):
    en.register_script(cls.__name__, cls)
    
    return cls


class Component:
    def on_update(self, ts):
        pass

@register_class
class Test3(Component):
    def __init__(self) -> None:
        print("Hello from test3")