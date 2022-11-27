
import engine_native as en
# import imgui

def register_class(cls):
    en.register_script(cls.__name__, cls)
    
    return cls


class Component:
    def on_update(self, ts):
        pass
    
    def on_imgui_update(self):
        pass


@register_class
class Test(Component):
    def __init__(self) -> None:
        self.i = float()
    
    def __del__(self):
        print("Del")
    
    def on_update(self, ts):
        self.i += ts
        # print("Hello!")
    
    def on_imgui_update(self):
        pass
        # imgui.Begin("Test")
        
        
        # imgui.End()
        
# print(dir(Test()))

