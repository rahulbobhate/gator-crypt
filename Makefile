#This is the Makefile which can be used manage the building of the utility.

# This is the command for carrying out all the actions mentioned in this Makefile.
all :
	make clean build

# This command compiles the code and creates two executable files gatorenc and gatordec.
build :
	gcc -o gatorenc src/GatorEncryptorMain.c src/GatorEncryptor.c src/GatorCryptInitializer.c src/GatorNetwork.c -I include/. `libgcrypt-config --cflags --libs`  
	gcc -o gatordec src/GatorDecryptorMain.c src/GatorDecryptor.c src/GatorCryptInitializer.c src/GatorNetwork.c -I include/. `libgcrypt-config --cflags --libs`  

# This command erases the output files gatorenc and gatordec.
clean : 
	$(RM) gatorenc
	$(RM) gatordec






