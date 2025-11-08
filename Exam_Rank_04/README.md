# GUÍA DE ESTUDIO EXAM_04 - PROCESOS Y PIPES

## 📋 RESUMEN EJECUTIVO

El exam_04 de 42 se centra en conceptos avanzados de **sistemas operativos** y **programación de bajo nivel**. Los temas principales son:

### TEMAS PRINCIPALES:
1. **Procesos y Fork** (picoshell, sandbox)
2. **Pipes y Comunicación** (pipelines)
3. **Parsing y Estructuras** (argo, vbc)
4. **Gestión de Memoria** (malloc/free)
5. **Manejo de Señales** (sandbox)

---

## 🎯 ESTRATEGIA DE ESTUDIO PASO A PASO

### FASE 1: FUNDAMENTOS (Días 1-3)
**Objetivo**: Dominar los conceptos básicos sin memorizar código

#### **DÍA 1: PROCESOS Y FORK**
```c
// Concepto clave: ¿Qué es un proceso?
pid_t pid = fork();
if (pid == 0) {
    // PROCESO HIJO
} else if (pid > 0) {
    // PROCESO PADRE  
} else {
    // ERROR
}
```

**Ejercicios mentales**:
- Dibuja el árbol de procesos para `fork(); fork();`
- ¿Cuántos procesos se crean con N forks?
- ¿Por qué usar `wait()` en el padre?

#### **DÍA 2: PIPES**
```c
// Concepto clave: Comunicación entre procesos
int pipefd[2];
pipe(pipefd);  // [0] = read, [1] = write
```

**Ejercicios mentales**:
- Dibuja el flujo: `ls | grep txt`
- ¿Cuándo cerrar cada descriptor?
- ¿Por qué usar `dup2()`?

#### **DÍA 3: PARSING BÁSICO**
```c
// Concepto clave: Análisis sintáctico
int accept(char **s, char c) {
    if (**s == c) {
        (*s)++;
        return 1;
    }
    return 0;
}
```

**Ejercicios mentales**:
- ¿Cómo parsear "2+3*4"?
- ¿Qué es la precedencia de operadores?
- ¿Cómo manejar paréntesis?

### FASE 2: EJERCICIOS INDIVIDUALES (Días 4-7)

#### **NIVEL 1: ft_popen**
**Concepto**: Crear un pipe bidireccional con un proceso

**Plan de implementación**:
1. **Validar parámetros**: file, argv, type
2. **Crear pipe**: `pipe(fd)`
3. **Fork proceso**
4. **En el hijo**: 
   - Redirigir stdin/stdout según `type`
   - `execvp(file, argv)`
5. **En el padre**: 
   - Cerrar descriptor no usado
   - Retornar descriptor correcto

**Puntos críticos**:
- `type='r'`: leer la salida del comando
- `type='w'`: escribir a la entrada del comando
- Cerrar descriptores para evitar leaks

#### **NIVEL 1: picoshell**
**Concepto**: Pipeline de comandos conectados

**Plan de implementación**:
1. **Bucle por comandos**: `while(cmds[i])`
2. **Crear pipe si hay siguiente**: `if(cmds[i+1]) pipe(fd)`
3. **Fork proceso**
4. **En el hijo**:
   - Configurar entrada: `dup2(prev_fd, STDIN)`
   - Configurar salida: `dup2(fd[1], STDOUT)`
   - `execvp(cmds[i][0], cmds[i])`
5. **En el padre**:
   - Gestionar descriptores
   - Guardar `prev_fd = fd[0]`
6. **Esperar todos los procesos**: `wait()`

**Puntos críticos**:
- Gestión correcta de descriptores
- No crear pipe para el último comando
- Esperar a TODOS los procesos hijo

#### **NIVEL 1: sandbox**
**Concepto**: Ejecutar función con timeout y capturar errores

**Plan de implementación**:
1. **Fork proceso**
2. **En el hijo**: 
   - `alarm(timeout)`
   - `f()`
   - `exit(0)`
3. **En el padre**:
   - Configurar handler para SIGALRM
   - `waitpid()` con manejo de EINTR
   - Analizar status de salida
4. **Determinar resultado**: nice/bad según exit code y señales

**Puntos críticos**:
- Manejo de señales (SIGALRM)
- Diferencia entre timeout y otros errores
- Evitar procesos zombie

### FASE 3: EJERCICIOS AVANZADOS (Días 8-10)

#### **NIVEL 2: vbc (Calculator Parser)**
**Concepto**: Parsear y evaluar expresiones matemáticas

**Gramática**:
```
expr   := term (('+') term)*
term   := factor (('*') factor)*  
factor := digit | '(' expr ')'
```

**Plan de implementación**:
1. **Parsing recursivo descendente**
2. **Construcción de AST** (Abstract Syntax Tree)
3. **Evaluación del árbol**

**Puntos críticos**:
- Precedencia: * antes que +
- Manejo de paréntesis
- Gestión de memoria del árbol

#### **NIVEL 2: argo (JSON Parser)**
**Concepto**: Parsear JSON completo

**Tipos soportados**:
- Integers: `-123`
- Strings: `"hello\"world"`  
- Maps: `{"key": value}`

**Plan de implementación**:
1. **Funciones de parsing por tipo**
2. **Gestión de memoria dinámica**
3. **Manejo de errores con cleanup**

**Puntos críticos**:
- Escape sequences en strings
- Arrays dinámicos para maps
- Free correcto en caso de error

---

## 🧠 TÉCNICAS DE COMPRENSIÓN (NO MEMORIZACIÓN)

### **1. VISUALIZACIÓN**
Siempre dibuja antes de programar:
```
ls | grep txt | wc -l

Proceso 1: ls
    stdout → pipe1[1] → pipe1[0] → stdin Proceso 2

Proceso 2: grep txt  
    stdout → pipe2[1] → pipe2[0] → stdin Proceso 3

Proceso 3: wc -l
    stdout → terminal
```

### **2. PATRONES MENTALES**

**Patrón Fork-Exec**:
```c
if (fork() == 0) {
    // Configurar entorno
    // execvp()
    // exit() si falla
}
// Padre: gestionar recursos
```

**Patrón Pipe**:
```c
pipe(fd);
if (fork() == 0) {
    close(fd[0]);           // Hijo: cerrar read
    dup2(fd[1], STDOUT);    // Redirigir stdout
    close(fd[1]);
    execvp(cmd);
}
close(fd[1]);               // Padre: cerrar write  
// Usar fd[0] para leer
```

**Patrón Parser**:
```c
static node *parse_X(char **s) {
    node *left = parse_Y(s);
    while (accept(s, '+')) {
        node *right = parse_Y(s);
        left = create_node(ADD, left, right);
    }
    return left;
}
```

### **3. DEBUGGING MENTAL**
Preguntas que hacer siempre:

- **Procesos**: ¿Cuántos procesos hay? ¿Quién hace qué?
- **Descriptores**: ¿Qué descriptores están abiertos? ¿Cuándo cerrarlos?
- **Memoria**: ¿Qué se aloca? ¿Cuándo se libera?
- **Errores**: ¿Qué puede fallar? ¿Cómo manejar cada error?

---

## 📚 RECURSOS DE PRÁCTICA

### **Ejercicios Mentales Diarios**
1. **Día par**: Dibuja pipelines complejas
2. **Día impar**: Parsea expresiones a mano

### **Preguntas de Autoevaluación**
1. ¿Puedo explicar fork() a un niño de 10 años?
2. ¿Puedo dibujar cualquier pipeline sin pensar?
3. ¿Entiendo por qué cerrar descriptores específicos?
4. ¿Puedo parsear cualquier gramática simple?

### **Señales de que Vas Bien**
✅ Puedes dibujar el flujo antes de programar
✅ Sabes qué descriptores cerrar sin dudar  
✅ Entiendes el propósito de cada `wait()`
✅ Puedes explicar la diferencia entre parsing y evaluation

---

## 🚀 CRONOGRAMA FINAL (DÍA DEL EXAMEN)

### **30 min antes**:
- Repasa los patrones mentales
- Dibuja un pipeline simple (calentamiento)
- Respira profundo

### **Durante el examen**:
1. **Lee el subject 2 veces**
2. **Dibuja la solución**
3. **Identifica el patrón**
4. **Programa paso a paso**
5. **Verifica casos extremos**

### **Si te bloqueas**:
- Vuelve al dibujo
- Simplifica el problema
- Programa la estructura básica primero

---

## 💡 CONSEJOS FINALES

**NO hagas**:
- ❌ Memorizar código completo
- ❌ Intentar múltiples ejercicios a la vez
- ❌ Ignorar la gestión de descriptores

**SÍ haz**:
- ✅ Entiende CADA línea de código
- ✅ Practica dibujar flujos
- ✅ Verifica siempre los casos de error
- ✅ Piensa en términos de patrones

**Recuerda**: El objetivo no es memorizar, sino **entender los conceptos fundamentales** para poder aplicarlos a cualquier variación del problema.
