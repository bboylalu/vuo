Creates a graphics shader that can paint a 3D object with a solid color.

   - `color` — The main (diffuse) color of the surface.
   - `highlightColor` — The color of shiny (specular) reflections.
   - `shininess` — How dull (0) or polished (1) the surface appears.

This shader causes the object to be lit by the lights in the scene.  (Parts of the object that face the light will be brighter than parts that face away from the light.)
