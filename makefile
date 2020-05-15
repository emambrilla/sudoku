SOURCE=./source
HDRS=./hdrs

all: $(MYTARGET)/sudoku

clean:
	@echo Borrando...
	@$(MYRM) $(MYTARGET)/sudoku$(MYEXE_SUFFIX)

$(MYTARGET)/sudoku: $(SOURCE)/sudoku.c
	@echo Compilando sudoku
	@$(MYCC) $(SOURCE)/sudoku.c -o $(MYTARGET)/sudoku
