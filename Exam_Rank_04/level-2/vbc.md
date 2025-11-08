# VBC - EXPLICACIÓN LÍNEA POR LÍNEA

## 📚 PARTE 1: LIBRERÍAS Y ESTRUCTURAS

### **¿Qué librerías necesitas?**

```c
#include <stdio.h>   // ✅ Para printf()
#include <stdlib.h>  // ✅ Para malloc(), calloc(), free()
#include <ctype.h>   // ✅ Para isdigit()
```

**⚠️ PROBLEMA en el código dado**:
```c
#include <malloc.h>  // ❌ MAL - No es estándar
```

**✅ SOLUCIÓN**:
```c
#include <stdlib.h>  // ✅ BIEN - Contiene malloc, calloc, free
```

---

### **La Estructura del Nodo (AST)**

```c
typedef struct node {
    enum {
        ADD,      // Tipo 0: Nodo de suma
        MULTI,    // Tipo 1: Nodo de multiplicación
        VAL       // Tipo 2: Nodo de valor (hoja)
    } type;
    
    int val;           // Solo se usa cuando type == VAL
    struct node *l;    // Hijo izquierdo
    struct node *r;    // Hijo derecho
} node;
```

**Explicación ULTRA SIMPLE**:

**¿Qué es un nodo?**
Un nodo es una cajita que puede ser de 3 tipos:

```
TIPO 1 - VAL (Hoja):
┌─────────┐
│ type:VAL│
│ val: 5  │  ← Guarda el número
│ l: NULL │
│ r: NULL │
└─────────┘

TIPO 2 - ADD (Operador +):
┌──────────┐
│ type:ADD │
│ val: --  │  ← No se usa
│ l: ●────┼──→ Hijo izquierdo
│ r: ●────┼──→ Hijo derecho
└──────────┘

TIPO 3 - MULTI (Operador *):
┌───────────┐
│ type:MULTI│
│ val: --   │  ← No se usa
│ l: ●─────┼──→ Hijo izquierdo
│ r: ●─────┼──→ Hijo derecho
└───────────┘
```

**Ejemplo visual para `3+4`**:
```
       ┌──────────┐
       │ type:ADD │
       │ val: --  │
       └────┬─┬───┘
            │ │
      ┌─────┘ └─────┐
      ▼             ▼
┌─────────┐   ┌─────────┐
│type:VAL │   │type:VAL │
│val: 3   │   │val: 4   │
└─────────┘   └─────────┘
```

---

## 🔧 PARTE 2: FUNCIONES AUXILIARES

### **Función 1: `new_node()` - Crear un nodo**

```c
// CÓDIGO DADO (CON BUG):
node *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));  // ❌ sizeof(n) está MAL
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}
```

**🐛 ¿Cuál es el bug?**
- `n` es una variable de tipo `node` (no un puntero)
- `sizeof(n)` da el tamaño correcto **por casualidad**
- Pero es confuso y puede causar warnings

**✅ CÓDIGO CORRECTO**:
```c
node *new_node(node n)
{
    node *ret = calloc(1, sizeof(node));  // ✅ O sizeof(*ret)
    if (!ret)
        return NULL;
    *ret = n;  // Copiar contenido de n a ret
    return ret;
}
```

**Explicación LÍNEA POR LÍNEA**:
```c
node *new_node(node n)  // Recibe un nodo por VALOR
{
    // Reservar memoria para UN nodo
    node *ret = calloc(1, sizeof(node));
    //          ^calloc inicializa a 0
    //             ^1 elemento
    //                ^tamaño de un nodo
    
    if (!ret)  // Si calloc falló (sin memoria)
        return NULL;  // Retornar error
    
    *ret = n;  // Copiar el nodo n al espacio reservado
    //  ^desreferenciar: acceder al contenido
    
    return ret;  // Retornar el puntero al nodo creado
}
```

**Ejemplo de uso**:
```c
node temp;
temp.type = VAL;
temp.val = 5;
temp.l = NULL;
temp.r = NULL;

node *nuevo = new_node(temp);  // Crea copia en el heap
// Ahora 'nuevo' apunta a un nodo con valor 5
```

---

### **Función 2: `destroy_tree()` - Liberar memoria**

```c
void destroy_tree(node *n)
{
    if (!n)  // Si el nodo es NULL
        return;  // No hacer nada
    
    if (n->type != VAL)  // Si NO es una hoja
    {
        // Liberar hijos recursivamente
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    
    free(n);  // Liberar el nodo actual
}
```

**Explicación ULTRA SIMPLE**:

Esta función borra un árbol **de abajo hacia arriba**:

```
Árbol:        +
            /   \
           3     4

Orden de borrado:
1. destroy_tree(+) → No es VAL, tiene hijos
2.   destroy_tree(3) → Es VAL, no tiene hijos → free(3)
3.   destroy_tree(4) → Es VAL, no tiene hijos → free(4)
4. free(+) → Ahora sí, liberar el nodo raíz
```

**¿Por qué este orden?**
- Primero borramos los hijos (hojas)
- Luego borramos el padre
- Si lo hacemos al revés, **perdemos** los punteros a los hijos

---

### **Función 3: `unexpected()` - Mostrar error**

```c
// CÓDIGO DADO (CON BUG):
void unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of file\n");  // ❌ MAL
}
```

**🐛 ¿Cuál es el bug?**
El subject dice: `"Unexpected end of input\n"` (no "file")

**✅ CÓDIGO CORRECTO**:
```c
void unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");  // ✅ BIEN
}
```

**Explicación**:
```c
unexpected('x');  // Imprime: "Unexpected token 'x'"
unexpected('\0'); // Imprime: "Unexpected end of input"
```

---

### **Función 4: `accept()` - Consumir carácter si coincide**

```c
// CÓDIGO DADO (CON BUG):
int accept(char **s, char c)
{
    if (**s)  // ❌ Solo verifica si hay carácter
    {
        (*s)++;
        return (1);
    }
    return (0);
}
```

**🐛 ¿Cuál es el bug?**
No verifica que el carácter sea el esperado `c`

**✅ CÓDIGO CORRECTO**:
```c
int accept(char **s, char c)
{
    if (**s == c)  // ✅ Verificar que sea el carácter correcto
    {
        (*s)++;  // Avanzar el puntero
        return 1;  // Éxito
    }
    return 0;  // No es el carácter esperado
}
```

**Explicación PASO A PASO**:
```c
char *input = "3+4";
char *ptr = input;

// ptr apunta a '3'
if (accept(&ptr, '3'))  // ¿*ptr es '3'?
{
    // SÍ → ptr avanza a '+'
    // Retorna 1
}

if (accept(&ptr, '+'))  // ¿*ptr es '+'?
{
    // SÍ → ptr avanza a '4'
    // Retorna 1
}

if (accept(&ptr, '*'))  // ¿*ptr es '*'?
{
    // NO → ptr NO avanza
    // Retorna 0
}
```

**¿Por qué `char **s`?**

```
MEMORIA:
┌───┬───┬───┬───┬───┐
│ 3 │ + │ 4 │ \0│   │
└───┴───┴───┴───┴───┘
  ↑
  ptr (variable que apunta al string)

Cuando pasamos &ptr:
- Podemos modificar ptr desde dentro de accept()
- (*s)++ mueve ptr al siguiente carácter
```

---

### **Función 5: `expect()` - Esperar carácter obligatorio**

```c
int expect(char **s, char c)
{
    if (accept(s, c))  // Intentar consumir 'c'
        return 1;  // Éxito
    
    unexpected(**s);  // Mostrar error
    return 0;  // Fallo
}
```

**Diferencia entre `accept()` y `expect()`**:

```c
// accept() - Opcional (no muestra error):
if (accept(&ptr, '+'))
    printf("Hay un +\n");
else
    printf("No hay +, pero está bien\n");

// expect() - Obligatorio (muestra error si falta):
if (expect(&ptr, ')'))
    printf("Hay un )\n");
else
    printf("ERROR: Falta )\n");  // expect ya mostró el error
```

---

## 🌳 PARTE 3: FUNCIONES DE PARSING

### **Función 6: `parse_primary()` - Elementos básicos**

**¿Qué parsea?**
- Números: `0`, `1`, `2`, ... `9`
- Expresiones entre paréntesis: `(3+4)`, `((2*5))`

```c
node *parse_primary(char **s)
{
    // CASO 1: Paréntesis '('
    if (**s == '(')
    {
        (*s)++;  // Consumir '('
        
        node *result = parse_addition(s);  // Parsear contenido
        
        if (!result || **s != ')')  // Error o falta ')'
        {
            destroy_tree(result);  // Liberar si había nodo
            unexpected(**s);       // Mostrar error
            return NULL;
        }
        
        (*s)++;  // Consumir ')'
        return result;
    }
    
    // CASO 2: Dígito (0-9)
    if (isdigit(**s))
    {
        node temp;
        temp.type = VAL;
        temp.val = **s - '0';  // Convertir '5' → 5
        temp.l = NULL;
        temp.r = NULL;
        
        (*s)++;  // Consumir dígito
        return new_node(temp);
    }
    
    // CASO 3: Carácter inesperado
    unexpected(**s);
    return NULL;
}
```

**Explicación VISUAL**:

**Caso 1 - Paréntesis `"(3+4)"`**:
```
Entrada: "(3+4)"
         ↑
         ptr

1. Ver '(' → Consumir
   ptr = "3+4)"
          ↑

2. Llamar parse_addition() recursivamente
   Esto parsea "3+4" y retorna el árbol:
       +
      / \
     3   4

3. Verificar ')' → Consumir
   ptr = ""
          ↑

4. Retornar el árbol de "3+4"
```

**Caso 2 - Dígito `"5"`**:
```
Entrada: "5"
         ↑
         ptr

1. isdigit('5') = true
2. Convertir '5' a 5:
   '5' - '0' = 53 - 48 = 5 (código ASCII)
3. Crear nodo VAL(5)
4. Avanzar ptr
5. Retornar nodo
```

**Caso 3 - Error `"x"`**:
```
Entrada: "x"
         ↑
         ptr

1. No es '(' → siguiente caso
2. isdigit('x') = false → siguiente caso
3. unexpected('x')
   → Imprime: "Unexpected token 'x'"
4. Retornar NULL
```

---

### **Función 7: `parse_multiplication()` - Operador `*`**

```c
node *parse_multiplication(char **s)
{
    // 1. Parsear primer operando
    node *left = parse_primary(s);
    if (!left)
        return NULL;
    
    // 2. Mientras haya '*'
    while (**s == '*')
    {
        (*s)++;  // Consumir '*'
        
        // 3. Parsear siguiente operando
        node *right = parse_primary(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        
        // 4. Crear nodo MULTI
        node temp;
        temp.type = MULTI;
        temp.l = left;
        temp.r = right;
        temp.val = 0;  // No se usa
        
        left = new_node(temp);
        if (!left)
        {
            destroy_tree(right);
            return NULL;
        }
        // 5. Repetir si hay más '*'
    }
    
    return left;
}
```

**Explicación PASO A PASO con `"2*3*4"`**:

```
Entrada: "2*3*4"
         ↑
         ptr

--- ITERACIÓN 1 ---
1. parse_primary() → VAL(2)
   left = VAL(2)
   ptr = "*3*4"
          ↑

2. **s == '*' → SÍ → Consumir
   ptr = "3*4"
          ↑

3. parse_primary() → VAL(3)
   right = VAL(3)
   ptr = "*4"
          ↑

4. Crear nodo:
   left = MULTI(VAL(2), VAL(3))
          *
         / \
        2   3

--- ITERACIÓN 2 ---
5. **s == '*' → SÍ → Consumir
   ptr = "4"
          ↑

6. parse_primary() → VAL(4)
   right = VAL(4)
   ptr = ""
          ↑

7. Crear nodo:
   left = MULTI(MULTI(2,3), VAL(4))
              *
            /   \
           *     4
          / \
         2   3

--- FIN ---
8. **s == '*' → NO
9. Retornar left
```

**Resultado: Asociatividad izquierda**
`2*3*4` se parsea como `(2*3)*4`

---

### **Función 8: `parse_addition()` - Operador `+`**

```c
node *parse_addition(char **s)
{
    // 1. Parsear primer operando (puede contener *)
    node *left = parse_multiplication(s);
    if (!left)
        return NULL;
    
    // 2. Mientras haya '+'
    while (**s == '+')
    {
        (*s)++;  // Consumir '+'
        
        // 3. Parsear siguiente operando
        node *right = parse_multiplication(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        
        // 4. Crear nodo ADD
        node temp;
        temp.type = ADD;
        temp.l = left;
        temp.r = right;
        temp.val = 0;
        
        left = new_node(temp);
        if (!left)
        {
            destroy_tree(right);
            return NULL;
        }
    }
    
    return left;
}
```

**Explicación PASO A PASO con `"2+3*4"`**:

```
Entrada: "2+3*4"
         ↑
         ptr

--- PASO 1 ---
1. parse_multiplication() para "2+3*4"
   
   a. parse_primary() → VAL(2)
      ptr = "+3*4"
             ↑
   
   b. **s == '*' → NO
   
   c. Retorna VAL(2)

left = VAL(2)

--- PASO 2 ---
2. **s == '+' → SÍ → Consumir
   ptr = "3*4"
          ↑

--- PASO 3 ---
3. parse_multiplication() para "3*4"
   
   a. parse_primary() → VAL(3)
      ptr = "*4"
             ↑
   
   b. **s == '*' → SÍ → Consumir
      ptr = "4"
             ↑
   
   c. parse_primary() → VAL(4)
      ptr = ""
             ↑
   
   d. Crear MULTI(3, 4)
      right = *
             / \
            3   4

--- PASO 4 ---
4. Crear nodo ADD:
   left = ADD(VAL(2), MULTI(3,4))
          +
         / \
        2   *
           / \
          3   4

--- FIN ---
5. **s == '+' → NO
6. Retornar left
```

**¡Fíjate!** → El `*` se evaluó primero (precedencia correcta)

---

### **Función 9: `parse_expr()` - Función principal**

```c
node *parse_expr(char **s)
{
    // 1. Parsear la expresión completa
    node *ret = parse_addition(s);
    
    if (!ret)
        return NULL;
    
    // 2. Verificar que se consumió TODO
    if (**s)  // Si quedan caracteres sin parsear
    {
        destroy_tree(ret);
        unexpected(**s);
        return NULL;
    }
    
    // 3. Éxito
    return ret;
}
```

**Explicación**:

Esta función es un **wrapper** que:
1. Inicia el parsing
2. Verifica que no sobren caracteres
3. Maneja errores

**Ejemplo de error**:
```
Entrada: "3+4)"
         ↑
         ptr

1. parse_addition() parsea "3+4"
   ret = ADD(3,4)
   ptr = ")"
          ↑

2. **s == ')' → HAY CARACTERES SOBRANTES
   
3. destroy_tree(ret)  → Liberar árbol
   unexpected(')')    → "Unexpected token ')'"
   return NULL        → Error
```

---

## 📊 PARTE 4: EVALUACIÓN

### **Función 10: `eval_tree()` - Calcular resultado**

```c
int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return eval_tree(tree->l) + eval_tree(tree->r);
        
        case MULTI:
            return eval_tree(tree->l) * eval_tree(tree->r);
        
        case VAL:
            return tree->val;
    }
}
```

**Explicación VISUAL para `3+4*5`**:

```
Árbol:
       +
      / \
     3   *
        / \
       4   5

Evaluación (POST-ORDER):

eval_tree(+)
├─ eval_tree(3) → 3
└─ eval_tree(*)
   ├─ eval_tree(4) → 4
   └─ eval_tree(5) → 5
   └─ return 4 * 5 = 20
└─ return 3 + 20 = 23
```

**Paso a paso**:
1. Evaluar hijo izquierdo: `eval_tree(VAL(3))` → `3`
2. Evaluar hijo derecho: `eval_tree(MULTI(4,5))`
   a. Evaluar hijo izquierdo: `eval_tree(VAL(4))` → `4`
   b. Evaluar hijo derecho: `eval_tree(VAL(5))` → `5`
   c. Multiplicar: `4 * 5` → `20`
3. Sumar: `3 + 20` → `23`

---

## 🎯 PARTE 5: MAIN

```c
int main(int argc, char **argv)
{
    // 1. Verificar argumentos
    if (argc != 2)
        return 1;  // Error: no hay expresión
    
    // 2. Parsear expresión
    char *input = argv[1];
    node *tree = parse_expr(&input);
    
    if (!tree)
        return 1;  // Error en parsing
    
    // 3. Evaluar y mostrar resultado
    printf("%d\n", eval_tree(tree));
    
    // 4. Liberar memoria
    destroy_tree(tree);
    
    return 0;  // Éxito
}
```

**Ejemplo de ejecución**:
```bash
./vbc "3+4*5"
# argc = 2
# argv[0] = "./vbc"
# argv[1] = "3+4*5"

# 1. Parsear → Árbol
# 2. Evaluar → 23
# 3. Imprimir → "23"
# 4. Liberar → memoria limpia
```

---

## 📋 RESUMEN: ¿QUÉ ESTÁ BIEN Y QUÉ HAY QUE CAMBIAR?

### ✅ **LO QUE ESTÁ BIEN**:
- Estructura `node` → Perfecto
- `destroy_tree()` → Perfecto
- `eval_tree()` → Perfecto
- `main()` → Perfecto

### ❌ **LO QUE HAY QUE CAMBIAR**:

1. **Librería**:
   ```c
   #include <malloc.h>  // ❌ Cambiar a:
   #include <stdlib.h>  // ✅
   ```

2. **Bug en `new_node()`**:
   ```c
   sizeof(n)    // ❌ Cambiar a:
   sizeof(node) // ✅
   ```

3. **Bug en `accept()`**:
   ```c
   if (**s)         // ❌ Cambiar a:
   if (**s == c)    // ✅
   ```

4. **Bug en `unexpected()`**:
   ```c
   "end of file"    // ❌ Cambiar a:
   "end of input"   // ✅
   ```

### 📝 **LO QUE HAY QUE COMPLETAR**:
- `parse_primary()` → Crear desde cero
- `parse_multiplication()` → Crear desde cero
- `parse_addition()` → Crear desde cero
- `parse_expr()` → Completar (solo falta el cuerpo)

---

## 💡 PREGUNTA FINAL PARA VERIFICAR

**¿Puedes explicar por qué este orden?**
```
parse_addition()
    ↓ llama a
parse_multiplication()
    ↓ llama a
parse_primary()
```

**Respuesta**: Porque quien se llama PRIMERO tiene MENOR precedencia. Como `parse_addition()` llama a `parse_multiplication()`, significa que primero resolvemos las multiplicaciones y luego las sumas. Así `3+4*5` se evalúa como `3+(4*5)` y no como `(3+4)*5`.

**¿Lo entiendes ahora?** 🎯