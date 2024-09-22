# Heroes3Im2Map

## Transforms images into Heroes of Might and Magic 3 Hota files.

### Requirements:

- C++ to compile. I'm not a C++ expert, so please try to compile it yourself until it works. I used C/C++ Optimizing Compiler Version 19.29.30154 on Windows 10.
- 7zip: Homm3 maps are compressed uzing gzip, so this program zips the resulting raw file using gzip
  
### Usage:

1. Create/Find an input image:
    - use the colors from the colors file. I used the colors from [potmdehex homm3tools](https://github.com/potmdehex/homm3tools/blob/master/h3m/h3mlib/h3m_structures/h3m_description.english.txt) to define the terrain,  however I reversed their R and B values (theirs were flipped).
    - i include a .gpl (GIMP palette) file with the colors and their names, so you can use GIMP and paing easily 
    - make sure the size of the image in pixels corresponds to the size of the homm3 map
    - **Important!** make sure that for each color in your image there are no places where a 2x2 pixel sized square cannot fit. Failure to do this will crash map creation at the end.
    - If the image doesn't have the colors defined above, euclidian distance is used to get closest colors. This sucks.
    
2. Create the map:
   
    ```Heroes3Im2Map.exe <file_id>  <source_image> <output_file>```
  
    <file_id>: specifies the size of the Hota Map: "S", "M", "L", "XL", "H", "XH", "G"

    <source_image>: image file, must have the pixel size equal to the tiles specified above (36x36, 72x72, ..., 252x253)

    <output_file>: name of the output file (a homm3 map)
  
3. Make the map usable:

    - open file using the map editor and see that it looks horrible
    - click anywhere on the map with the terrain editor, and it will attemp to fix the mess automatically
    - **Important!** if there are places in your image where, for any color, a 2x2 pixel square cannot fit, you will get an error message and crash.
    - if everything worked, now you can add everything else, castles, heroes, objects.

### Acknowledgements

This tool wouldn't be possible without the instructions this repo: [potmdehex homm3tools](https://github.com/potmdehex/homm3tools/blob/master/h3m/h3mlib/h3m_structures/h3m_description.english.txt)



