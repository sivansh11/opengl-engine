import engine_type as et
import engine_math as em
import engine_native as en


def register_class(cls):
    en.register_script(cls.__name__, cls)
    
    return cls


class Component:
    def on_update(self, ts):
        pass
    
    def on_imgui_update(self):
        pass