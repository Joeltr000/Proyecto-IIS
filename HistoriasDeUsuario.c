//HU-03 Agregar articulo
int agregar_articulo(Superviviente *s, const char *nombre_art, int cantidad, int valor_unitario){
    if(s == NULL || cantidad <= 0 || valor_unitario <= 0){
        return 0;
    }

    //si ya existe acumula cantidad
    for(int i = 0; i < s->num_articulos; i++){
        if(strcmp(s->inventario[i].nombre, nombre_art) == 0){
            s->inventario[i].cantidad += cantidad;
            s->inventario[i].valor_unitario = valor_unitario;
            return 1;
        }
    }
    //si no existe, crece inventario
    if(s->num_articulos == s->capacidad_inv){
        int nueva_cap = (s->capacidad_inv == 0) ? 2 : s->capacidad_inv * 2;
        Articulo *temp = realloc(s->inventario, nueva_cap * sizeof(Articulo));
        if(temp == NULL){
            return 0;
        }
        s->inventario = temp;
        s->capacidad_inv = nueva_cap;
    }
    Articulo *nuevo = &S->inventario[S->num_articulos];
    strcpy(nuevo->nombre, nombre_art);
    nuevo->cantidad = cantidad;
    nuevo->valor_unitario = valor_unitario;
    
    return 1;
}

//wrapper para pedir datos por teclado HU-03
void agregar_articulo_menu(SistemaTrueque *sys){
    if(sys->num_usuarios == 0){
        printf("No hay  supervivientes registrados.\n");
        return;
    }
    int id = leer_entero("ID del superviviente");
    int id = buscar_por_id(sys, id);
    if(idx == -1){
        printf("No existe superviviente con ID %d\n", id);
        return;
    }

    char nombre[30];
    printf("Nombre del articulo");
    leer_texto(nombre, sizeof(nombre));

    int camtidad = leer_entero("cantidad");
    int valor = leer_entero("valor_unitario");

    if(agregar_articulo(&sys->usuarios[idx], nombre, cantidad, valor)){
        printf("Articulo agregado a %s\n", sys->usuarios[idx].nombre);
    }else{
        printf("Error al agregar articulo\n");
    }
}

//HU-04 Mostrar sistema 
void mostrar_sistema(const SistemaTrueque *sys){
    printf("sistema de trueque \n");
    printf("Total supervivientes: %d\n", sys->num_usuarios);
    printf("Total transacciones: %d\n", sys->num_tx);

    if(sys->num_usuarios == 0){
        printf("Sin supervivientes registrados \n");
        return;
    }
    for(int i = 0; i < sys->num_usuarios; i++){
        const Superviviente *s = &sys->usuarios[i];
        printf("[%d] %s", s->id, s->nombre);
        if(s->num_articulos == 0){
            printf("Sin articulos\n");
        }else{
            for(int j = 0; j < s->num_articulos; j++){
                printf("%s | cantd: %d | valor: %d | total: %d\n",
                s->inventario[j].nombre,
                s->inventario[j].cantd,
                s->inventario[j].valor_unitario,
                s->inventario[j].cantidad * s->inventario[j].valor_unitario);
            }
        }
    }
    printf("\n");
}

//HU-05
int evaluar_viabilidad(int valor_ofecido, int valor_solicitado){
    if(valor_solicitado <= 0 || valor_ofecido < 0){
        return 0;
    }
    int diferencia = valor_ofecido - valor_solicitado;
    if(diferencia < 0){
        diferencia = -diferencia;
    }
    //evita flotantes: diferencia * 100 <= solicitado * 20
    return(diferencia * 100 <= valor_solicitado * 20);
}

void probar_viabilidad_menu(void){
    int ofrecido = leer_entero("valor total ofrecido");
    int solicitado = leer_entero("valor total solicitado");

    int viable = evaluar_viabilidad(ofrecido, solicitado);
    int dif = ofrecido - solicitado;
    if(dif < 0) dif = -dif;

    printf("Diferencia: %d\n", dif);
    if(viabilidad){
        printf("Resultado: viable se puede realizar el trueque\n");
    }else{
        printf("Resultado: no viable diferencia mayor al 20%\n");
    }
}

//LIBERAR MEMORIA
void liberar_sistemas(SistemaTrueque *sys){
    for(int i = 0; i < sys->num_usuarios; i++){
        free(sys->usuarios[i].inventario);
        sys->usuarios[i].inventario = NULL;
    }
    free(sys->usuarios);
    sys->usuarios = NULL;
    free(sys->historial);
    sys->historial = NULL;
    sys->num_usuarios = 0;
    sys->num_tx = 0;
    sys->cap_usuarios = 0;
    sys->cap_tx = 0;
}

HU-00 Main
int main(void){
    SistemaTrueque sys;
    inicializar_sistema(&sys);

    int opcion;
    do{
        printf("MENU Trueque\n");
        printf("Registrar superviviente \n");
        printf("Agregar articulo \n");
        printf("Mostrar sistema \n");
        printf("Evaluar viabilidad \n");
        printf("0. salir \n");
        opcion = leer_entero("opcion");

        switch(opcion){
            case 1: registrar_superviviente(&sys); break;
            case 2: agregar_articulo_menu(&sys); break;
            case 3: mostrar_sistema(&sys); break;
            case 4: probar_viabilidad_menu() break;
            case 0: printf("Saliendo \n"); break;
            default: printf("opcion invalida \n"); break;
        }
    }while (opcion !=0);

    liberar_sistemas(&sys);
    return 0;
}