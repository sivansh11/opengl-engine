# TODO

- fix texture mipmapping 
- seperate rendering thread 
- actually good camera controller
- editor camera


- models and meshes properly


# Dynamic Material System

- MaterialTypeBuilder defines the layout of the material     
- on calling build, the memory is initialized

# Renderer system

- each renderer will have its own material type defination (dynamically defined)
- each material will know all meshes that uses them to render


```
for shader in all shaders
    bind shader
    for material in all materials for the bound shader
        bind material
        for mesh in all mesh with this material
            bind mesh
            draw mesh
```