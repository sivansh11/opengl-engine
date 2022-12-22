# TODO

<!-- - ecs update -->
<!-- - actually good camera controller -->
<!-- - entity and scene class -->
<!-- - restructure -->
<!-- - better content browser -->
- rework scripting, atm its hacked in
- sky box
- selected entity panel should show sub meshes
- billboards
- billboards lights
- dsa program shader (look at the dsa docs fully once)
- uniform buffers
- asset manager
- shader rework, same file shaders, shader options
- fix space below menu bar
- actually make src files for headers smh this already taking too long to compile
- scene manager to comunicate with the script system
- bvh
- shadows
- raytracing 
- raytraced visible tris
- raytraced shadows
- frame arenas
- add script manager and reloading
- start, pause, restart
- viewports
- documentation for python
- readme
- events
- python based systems
- render system
- dynamic material system
- rename stuff / folder ??
- create a logo
- seperate rendering thread 
- textures for models with no textures properly


# Dynamic material system

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