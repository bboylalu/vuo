Turns an image into a 3D object that can be added to a 3D scene. 

The 3D object is like a piece of paper that displays the image on one side.

   - `center` — The center point of the 3D object, in Vuo coordinates.
   - `rotation` — The rotation of the 3D object, in degrees (Euler angles). At (0,0,0), the image is facing front.
   - `width` — The width of the 3D object, in Vuo coordinates. The height is calculated from the width to preserve the image's aspect ratio.
   - `alpha` — The opacity of the 3D object. Ranges from 0 (fully transparent) to 1 (fully opaque). 
   - `highlightColor` — The color of shiny (specular) reflections.
   - `shininess` — How dull (0) or polished (1) the surface appears.

This object is lit by the lights in the scene.  (Parts of the object that face the light will be brighter than parts that face away from the light.)
