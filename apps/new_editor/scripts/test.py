import sys
sys.path.append("../../../")   # to import from a different file lmao 
import engine as e


@e.register_class
class Test(e.Component):
    def __init__(self, ent) -> None:
        self.ent = ent
        if not self.ent.has_transform():
            self.ent.assign_transform()
        self.multiplyer = e.em.vec1(1.0)
        
    def on_update(self, ts):
        transform = self.ent.get_transform()
        move = e.em.vec3(0)
        if (e.en.is_key_down(e.et.Key.Y)):
            move += e.em.vec3(1, 0, 0)
        if (e.en.is_key_down(e.et.Key.H)):
            move -= e.em.vec3(1, 0, 0)
        if (e.en.is_key_down(e.et.Key.G)):
            move -= e.em.vec3(0, 0, 1)
        if (e.en.is_key_down(e.et.Key.J)):
            move += e.em.vec3(0, 0, 1)
        if (e.en.is_key_down(e.et.Key.T)):
            move += e.em.vec3(0, 1, 0)
        if (e.en.is_key_down(e.et.Key.U)):
            move -= e.em.vec3(0, 1, 0)
        transform.translation += move * ts * self.multiplyer.x
        
    def on_imgui_update(self):
        e.en.begin("Test Panel")
        e.en.drag_float("multiplyer", self.multiplyer, float(1), float(0), float(100))
        e.en.end()