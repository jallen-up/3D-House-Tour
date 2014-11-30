Object Parser Read Me

Here is a quick read me to help you through some problems that you might encounter with the parser.  

Tips and Tricks:

Setting up the .obj file correctly
- Once you have created your desired object in blender,  slide out a UV viewport and create a new texture image.
  This image can be blank, but it needs to be created to properly map the texture coordinates to the object.
  The object file stores each vertex as VV\TT\NN V= vertex location, T = Texture location, N = normal vector.
  If you do not create the image, there will be no texture coordiantes and the parser will break. 

- Remember to check "write normals" and "triangulate faces" when exporting your .obj file! 

- Make sure your normals are flipped the right way in blender.  In edit mode, using face select, highlight
  the face that appears to be wrong.  Hit the W key, then the F key to flip your normal. (you will notice this
  easily when the lighting is implemented)

- If you delete or disolve vertacies and faces in blender, sometimes it will leave invisable artifacts that show up
  at the end of the .obj file.  Just go ahead and delete these and everything should work fine.    

