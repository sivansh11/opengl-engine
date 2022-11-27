# TODO

<!-- - ecs update -->
<!-- - actually good camera controller -->
<!-- - entity and scene class -->
<!-- - restructure -->
- fix space below menu bar
- actually make src files for headers smh this already taking too long to compile
- scene manager to comunicate with the script system
- add script manager and reloading
- start, pause, restart
- better content browser
- viewports
- documentation for python
- readme
- events
- python based systems
- render system
- rename stuff / folder ??
- create a logo
- seperate rendering thread 
- textures for models with no textures properly


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