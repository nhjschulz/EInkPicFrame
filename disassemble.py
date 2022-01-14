Import("env", "projenv")

env.AddPostAction(
	"$BUILD_DIR/${PROGNAME}.elf",
	env.VerboseAction("avr-objdump -h -l -S $BUILD_DIR/${PROGNAME}.elf > $PROJECT_DIR/${PROGNAME}.lss", 
	"Creating assembler listing file $PROJECT_DIR/${PROGNAME}.lss")
)