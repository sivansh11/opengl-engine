# TODO

<!-- - ecs update -->
<!-- - actually good camera controller -->
- scene manager to comunicate with the script system
- add script manager and reloading
- better content browser
- add script reloading
- entity and scene class
- seperate rendering thread 
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