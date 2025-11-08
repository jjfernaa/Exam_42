# TUTORIAL PASO A PASO: PICOSHELL

## 🎯 OBJETIVO
Implementar `int picoshell(char **cmds[])` que ejecute una pipeline de comandos conectados por pipes.

---

## 📋 ANÁLISIS DEL PROBLEMA

### **¿Qué necesitamos hacer?**
```bash
# Ejemplo: ls | grep txt | wc -l
cmds[0] = ["ls", NULL]
cmds[1] = ["grep", "txt", NULL]  
cmds[2] = ["wc", "-l", NULL]
cmds[3] = NULL
```

### **Flujo de datos**:
```
ls (proceso1) → pipe1 → grep (proceso2) → pipe2 → wc (proceso3) → stdout
```

---

## 🧠 COMPRENSIÓN CONCEPTUAL

### **¿Por qué necesitamos pipes?**
- Los procesos no pueden comunicarse directamente
- Un pipe conecta la salida de un proceso con la entrada del siguiente
- `pipe()` crea dos descriptores: `fd[0]` (read) y `fd[1]` (write)

### **¿Por qué necesitamos fork?**  
- Cada comando debe ejecutarse en su propio proceso
- `fork()` crea una copia del proceso actual
- `execvp()` reemplaza el programa del proceso hijo

### **¿Cómo conectar procesos?**
- `dup2()` redirige un descriptor de archivo
- `dup2(fd[1], STDOUT_FILENO)`: la salida va al pipe
- `dup2(fd[0], STDIN_FILENO)`: la entrada viene del pipe

---

## 🔧 IMPLEMENTACIÓN PASO A PASO

### **PASO 1: Variables y Estructura Base**

```c
int picoshell(char **cmds[])
{
    int i = 0;              // Índice del comando actual
    int prev_fd = -1;       // Descriptor del pipe anterior  
    int pipefd[2];          // Para crear nuevos pipes
    pid_t pid;              // ID del proceso hijo
    int status;             // Para wait()
    
    // ... resto del código
    
    return 0;  // Éxito
}
```

**¿Por qué estas variables?**
- `i`: Para iterar por todos los comandos
- `prev_fd`: Guarda la salida del comando anterior
- `pipefd[2]`: Para crear la conexión con el siguiente comando
- `pid`: Para identificar proceso padre/hijo
- `status`: Para verificar el éxito de los comandos

### **PASO 2: El Bucle Principal**

```c
while (cmds[i])  // Mientras haya comandos
{
    // ¿Hay un comando siguiente?
    if (cmds[i + 1] != NULL) {
        // SÍ: Crear pipe para conectar con el siguiente
        if (pipe(pipefd) == -1)
            return 1;  // Error
    }
    
    // Crear proceso hijo
    pid = fork();
    if (pid == -1)
        return 1;  // Error en fork
        
    if (pid == 0) {
        // CÓDIGO DEL PROCESO HIJO
        // ...
    } else {
        // CÓDIGO DEL PROCESO PADRE  
        // ...
    }
    
    i++;  // Siguiente comando
}
```

**Flujo mental**:
1. ¿Hay siguiente comando? → Crear pipe
2. Fork → Dividir en padre e hijo  
3. Hijo → Configurar y ejecutar
4. Padre → Gestionar recursos

### **PASO 3: Configuración del Proceso Hijo**

```c
if (pid == 0) {  // Proceso hijo
    // ENTRADA: ¿Hay pipe anterior?
    if (prev_fd != -1) {
        // SÍ: Leer del pipe anterior
        dup2(prev_fd, STDIN_FILENO);
        close(prev_fd);
    }
    // Si no hay prev_fd, usar stdin normal (terminal)
    
    // SALIDA: ¿Hay comando siguiente?
    if (cmds[i + 1] != NULL) {
        // SÍ: Escribir al pipe siguiente
        close(pipefd[0]);  // No necesitamos leer
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
    }
    // Si no hay siguiente, usar stdout normal (terminal)
    
    // EJECUTAR el comando
    execvp(cmds[i][0], cmds[i]);
    exit(1);  // Solo se ejecuta si execvp falla
}
```

**Lógica visual**:
```
Comando N:
  stdin  ← prev_fd (del comando N-1)  
  stdout → pipefd[1] (para comando N+1)

Primer comando (N=0):
  stdin  ← terminal (prev_fd == -1)
  stdout → pipefd[1]

Último comando:
  stdin  ← prev_fd  
  stdout → terminal (no hay cmds[i+1])
```

### **PASO 4: Gestión del Proceso Padre**

```c
else {  // Proceso padre
    // Cerrar el pipe anterior (ya no lo necesitamos)
    if (prev_fd != -1)
        close(prev_fd);
        
    // Si creamos pipe nuevo, gestionar descriptores
    if (cmds[i + 1] != NULL) {
        close(pipefd[1]);       // El hijo usa el write end
        prev_fd = pipefd[0];    // Guardamos read end para siguiente
    }
    // Cerrar los dos pipefd[] al final
    else
    {
        close(pipefd[0]);
        close(pipefd[1]);
    }
}
```

**¿Por qué cerrar descriptores?**
- `prev_fd`: El padre ya no lo necesita, el hijo ya lo configuró
- `pipefd[1]`: Solo el hijo debe escribir, evitar deadlocks
- `prev_fd = pipefd[0]`: Para que el siguiente comando pueda leer

### **PASO 5: Esperar a Todos los Procesos**

```c
// Después del bucle while:
while (wait(&status) > 0) {
    // Verificar si algún comando falló
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        return 1;  // Algún comando retornó error
    if (!WIFEXITED(status))  // Terminado por señal
        return 1;
}
```

**¿Por qué wait() múltiple?**
- Hay N procesos hijo (uno por comando)
- Debemos esperar a que TODOS terminen
- Si alguno falla, el pipeline falla

---

## 🔍 CÓDIGO COMPLETO EXPLICADO

```c
int picoshell(char **cmds[])
{
    int i = 0;
    int prev_fd = -1;
    int pipefd[2];  
    pid_t pid;
    int status;

    while (cmds[i])
    {
        // Crear pipe si hay comando siguiente
        if (cmds[i + 1] && pipe(pipefd) == -1)
            return 1;

        pid = fork();
        if (pid == -1)
            return 1;

        if (pid == 0)  // Hijo
        {
            // Configurar entrada
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            // Configurar salida
            if (cmds[i + 1])
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        else  // Padre
        {
            if (prev_fd != -1)
                close(prev_fd);
            if (cmds[i + 1])
            {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
            else
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }
        }
        i++;
    }

    // Esperar todos los procesos
    while (wait(&status) > 0)
    {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            return 1;
        if (!WIFEXITED(status))
            return 1;
    }

    return 0;
}
```

---

## 🎨 EJEMPLO VISUAL: `ls | grep txt`

### **Iteración 1: Comando `ls`**
```
1. cmds[0] = ["ls", NULL], cmds[1] = ["grep", "txt", NULL]
2. Hay siguiente → pipe(pipefd) → pipefd[0], pipefd[1]  
3. fork() → padre e hijo

HIJO (ls):
  prev_fd == -1 → stdin viene del terminal
  hay siguiente → stdout va a pipefd[1]
  execvp("ls", ["ls", NULL])

PADRE:
  close(pipefd[1]) → solo hijo escribe
  prev_fd = pipefd[0] → guardamos para grep
```

### **Iteración 2: Comando `grep`**  
```
1. cmds[1] = ["grep", "txt", NULL], cmds[2] = NULL
2. No hay siguiente → no crear pipe
3. fork() → padre e hijo

HIJO (grep):
  prev_fd != -1 → stdin viene de prev_fd (salida de ls)
  no hay siguiente → stdout va al terminal  
  execvp("grep", ["grep", "txt", NULL])

PADRE:
  close(prev_fd) → ya no lo necesitamos
```

### **Final: Wait**
```
wait() → espera a que termine ls
wait() → espera a que termine grep
return 0 → éxito
```

---

## 🚀 CONSEJOS PARA EL EXAMEN

### **Antes de programar**:
1. **Dibuja el flujo** con 2-3 comandos
2. **Identifica** qué descriptores necesita cada proceso
3. **Planifica** cuándo cerrar cada descriptor

### **Durante la programación**:
1. **Estructura base** primero (variables, bucle)
2. **Proceso hijo** segundo (configuración + exec)
3. **Proceso padre** tercero (gestión descriptores)
4. **Wait** al final

### **Casos extremos**:
- ✅ Un solo comando: `cmds = [["ls", NULL], NULL]`
- ✅ Comando inválido: `execvp` falla
- ✅ Error en pipe/fork: retornar 1

### **Errores comunes**:
- ❌ No cerrar descriptores → deadlock
- ❌ Cerrar en orden incorrecto → pipes rotos
- ❌ No esperar a todos los hijos → zombies
- ❌ No verificar errores → comportamiento indefinido

---

## 🎯 AUTOEVALUACIÓN

**¿Entiendes realmente?**
- [ ] Puedo dibujar el flujo para cualquier pipeline
- [ ] Sé por qué cada descriptor se cierra en cada momento
- [ ] Entiendo la diferencia entre hijo y padre en cada iteración  
- [ ] Puedo explicar qué pasa si no hago `wait()`

**¿Estás listo?**
Si puedes responder SÍ a todo lo anterior, ¡estás listo para implementar picoshell en el examen! 🚀
