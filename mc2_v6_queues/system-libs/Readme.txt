This directory was merged from following sources andrestructured to inc/ and src/ directory for each library:

- 'freertos' was extracted from FreeRTOS V9.0.0 project source (gcc cortex M4F) and restructered to .

- 'stm32f429i-discovery' was extracted from the STM32Cube_FW_F4_V1.14.0 Firmware zip file and restructered accordingly.

- The remaining content was extracted from a 'gnuarmeclipse' sample project by first installing the Eclipse extension 'gnuarmeclipse' 
  then creating a cortex M4 sample project (see http://gnuarmeclipse.github.io/).
  Each library was then restructured to inc/ and src/ folders.
  (The 'diag' sources are not used because they were too weak)

Btw: 'gnuarmeclipse' projects uses automatically by Eclipse managed Makefiles but because this has too many traps, 
	a manual Makefile was then created for the mc2 course template. 


 Matthias Meier, 2017