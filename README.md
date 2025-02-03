# **Compilador C-**

## **Descrição**
O compilador foi dividido em três partes: **análise léxica, análise sintática** e **análise semântica**. O arquivo `compiler.c` é responsável por chamar essas etapas para analisar um código escrito em C-. As flags descritas na seção de execução do programa permitem executar cada uma dessas partes individualmente.

### **1. Análise Léxica**
A análise léxica foi implementada com base em estados. O compilador determina tokens a partir do caractere atual, utilizando uma enumeração (`TokenType`) que contém todos os tokens da linguagem C-. Para **Números (NUM)** e **Identificadores (ID)**, a análise é feita diretamente no lexema. Além disso, a análise léxica ignora comentários delimitados por `/* */`.

### **2. Análise Sintática**
A análise sintática utiliza a técnica de **análise recursiva descendente**, seguindo a gramática do livro do Louden, com algumas modificações. A geração da **árvore sintática** não está completamente implementada, mas a análise sintática em si está funcional para códigos escritos em C-.

### **3. Análise Semântica**
A análise semântica é realizada em duas passagens sobre o vetor de tokens gerado na análise léxica:
1. Construção da **tabela de símbolos** e detecção de **declarações repetidas**.
2. Execução do restante da análise semântica.

---
## **Instruções de Execução**

### **Compilação do Compilador**
#### **1. Criar o executável sem árvore sintática**
```bash
gcc -Wall -o compiler compiler.c lexer/lexer.c lexer/funcs.c parser/parser.c parser/funcs.c semantico/semantico.c semantico/funcs.c
```

---
## **Execução do Compilador**

### **1. Criar um código de teste**
Crie um arquivo de código-fonte C- e coloque-o na raiz do repositório.

### **2. Rodar o compilador com as flags desejadas**
As flags disponíveis permitem a execução individual de cada etapa do compilador:
- **`-l` / `-L`** → Executa a análise léxica e imprime os tokens.
- **`-p` / `-P`** → Executa a análise sintática, sem impressão da árvore sintática.
- **`-s` / `-S`** → Executa a análise semântica e imprime a tabela de símbolos.

#### **Exemplo de execução:**
```bash
./compiler seu_arquivo_de_teste.c -l
./compiler seu_arquivo_de_teste.c -L
./compiler seu_arquivo_de_teste.c -p
./compiler seu_arquivo_de_teste.c -P
./compiler seu_arquivo_de_teste.c -s
./compiler seu_arquivo_de_teste.c -S
```

---
## **Execução da Análise Sintática com Árvore Sintática**
Se desejar rodar a análise sintática com a formação da árvore sintática:

### **1. Criar o executável específico**
```bash
gcc -Wall -o compiler2 compiler.c lexer/lexer.c lexer/funcs.c parser_tree/parser_tree.c parser_tree/funcs_tree.c semantico/semantico.c semantico/funcs.c
```

### **2. Executar o compilador**
```bash
./compiler2 seu_arquivo_de_teste.c -p
./compiler2 seu_arquivo_de_teste.c -P
```

---
## **Conclusão**
Este compilador C- permite a análise de códigos utilizando diferentes níveis de verificação (léxico, sintático e semântico). Com as instruções acima, você pode compilar e executar o compilador conforme necessário para testar programas escritos nessa linguagem.

