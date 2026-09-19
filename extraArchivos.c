#include <time.h>
#include <conio.h>
#include <string.h> /*Bibliotecas*/
#include <stdio.h> 
#include <stdlib.h>

	/* Estructuras */
	struct record{
		char nombre[50]; 
		int puntaje;
		char fecha[30]; //Fecha / cuando se registro 
	} *jugadores;

	struct disparo{
		int fila;
		int columna;
	};
	
	typedef struct juego{
		char *Tablero; 		//Miembros
		int estado;
		int juegoIniciado;
		struct disparo *secuencia; 
		int cant_tiros;
		int esp; //tamaño del tablero guardado en la estructura
		char nombreJugador[50]; // nombre del jugador actual 
	} Juego;

	/* Prototipos de Funciones */
	void iniciar_juego(Juego *,int); 
	void mostrar_tablero(Juego *,int);
	void realizar_disparos(Juego *,int);
	void mostrar_ranking(struct record *,int); 
	void mostrar_secuencia_disparo(Juego *);
	int continuar_juego(Juego *, int *);
	void limpiarBuffer();
	
	/* Prototipos de Persistencia */
	void cargar_ranking(int *total);
	void guardar_ranking(char *nombre, int puntaje);
	void mostrar_mejor_jugador(int total);
	void guardar_historial_disparo(char *nombre, int fila, int columna, char *resultado);
	void guardar_partida(Juego *j);
	int cargar_partida(Juego *j);

	/* Arranca el Programa Principal */	
int main() 
{
	//Variables Locales
	int OPC;
	int espacio = 0;
	int total_jugadores = 0;
	Juego *juego = NULL; 	 // puntero local a la estructura
	
	//Reservar la estructura antes de usarla
	juego = malloc(sizeof(Juego));
	if(juego == NULL){
		puts("**ERROR al reservar juego**");
		return 1;
	}
	// Inicializar miembros
	juego->secuencia = NULL;
	juego->Tablero = NULL;
	juego->juegoIniciado = 0;
	juego->estado = 0;
	juego->cant_tiros = 0;
	juego->esp = 0;
	juego->nombreJugador[0] = '\0';
	
	jugadores = NULL; //Inicio en NULL
	
	//Inicio de nros aleatorios
	srand(time(NULL)); 
	
	//Cargo el archivo desde el inicio
	cargar_ranking(&total_jugadores);
	do{
		/* **MENU** */
		printf("\n\t\tBatalla Naval\n\n");
		
		/*Incentivo del mejor jugador*/
		mostrar_mejor_jugador(total_jugadores);
		
		printf("1. Iniciar Juego\n");
		printf("2. Mostrar Tablero\n");
		printf("3. Realizar Disparo\n");
		printf("4. Mostrar Ranking\n");
		printf("5. Secuencia de Disparos\n");
		printf("6. Guardar Partida\n");
		printf("7. Cargar Partida\n");
		printf("8. Salir\n");
		
		if(juego->juegoIniciado == 1){
			printf("\n\t\t\t\t\t\t\tDisparos: %d || 10",juego->cant_tiros); //Se cuentan la cantidad de disparos
		}
		
		printf("\n*<Elija una opcion>*\n");
		scanf("%d",&OPC);
		limpiarBuffer();
		system("cls");
		
		switch(OPC){
			
		case 1: //Inicio de Tablero y Juego																	
			if(juego->juegoIniciado == 0){
				do{
					printf("Ingrese el tamaño del tablero: ");
					scanf("%d",&espacio);
					limpiarBuffer();
				} while(espacio<=0);
				
				juego->Tablero = malloc(espacio * espacio * sizeof(char)); //Reservo memoria para tablero espacio*espacio*1byte
				
				if(juego->Tablero == NULL){ //En caso de falla de memoria
					puts("**ERROR**"); 
					return 0;
				}
				juego->esp = espacio;
				
				printf("Ingresa tu nombre de jugador: ");
				fgets(juego->nombreJugador, sizeof(juego->nombreJugador), stdin);
				juego->nombreJugador[strcspn(juego->nombreJugador, "\n")] = '\0';
				
				iniciar_juego(juego,espacio);
				juego->juegoIniciado = 1; //Cambio el estado del juego
				
			}else{
				puts("\n<<Ya inicio el juego>>");
			}
			break;
			
		case 2: //Se muestra el Tablero
			if(juego->juegoIniciado == 1){
				mostrar_tablero(juego,juego->esp);  //Muestra la posicion del barco ahora
				
			}else{
				puts("\n<<Aun no Inicia el Juego>>");
			}
			break;
			
		case 3: //Realiza Disparo
			if(juego->cant_tiros >= 10){
				puts("\n\t<<Sin Disparos restantes>>"); /*Partida Perdida*/
				puts("\n\t<<Volver a Intentar>>");
				juego->juegoIniciado = 0;
				juego->cant_tiros = 0;
				break;
			}
			if(juego->juegoIniciado == 1){
				realizar_disparos(juego, juego->esp); 
				
				if(juego->estado == 1){
					puts("\n\n\t<<FELICIDADES, Has Ganado!!>>");
					mostrar_tablero(juego, juego->esp);
					
					jugadores = realloc(jugadores, (total_jugadores + 1) * sizeof(struct record));
					if(jugadores == NULL){
						puts("Error de memoria");
						exit(1);
					}
					
					strcpy(jugadores[total_jugadores].nombre, juego->nombreJugador);
					jugadores[total_jugadores].puntaje = juego->cant_tiros;
	
					/* Fecha Leida */
					time_t tiempoActual;
					struct tm *infoTiempo;
					time(&tiempoActual);
					infoTiempo = localtime(&tiempoActual);
					strftime(jugadores[total_jugadores].fecha,
							 sizeof(jugadores[total_jugadores].fecha),
							 "%d/%m/%Y - %H:%M:%S", infoTiempo);
					
					total_jugadores++;
					
					guardar_ranking(juego->nombreJugador, juego->cant_tiros);
					printf("\n\n");
				}
			}else{
				puts("\n<<Aun no Inicia el Juego>>");
			}
			
			break;
		case 4: //Muestra ranking
			mostrar_ranking(jugadores, total_jugadores);
			
			break;
		case 5: //Almacenar secuencia de disparos
			mostrar_secuencia_disparo(juego);
			
			break;
		case 6: //Guardar Partida
			if(juego->juegoIniciado == 1){
				guardar_partida(juego);
			}else{
				puts("\n<<No hay partida activa para guardar>>");
			}
			break;
		case 7: //Cargar Partida
			if(juego->juegoIniciado == 0){
				if(cargar_partida(juego)){
					espacio = juego->esp;
					puts("\n<<Partida cargada correctamente>>");
				}else{
					puts("\n<<No se pudo cargar la partida>>");
				}
			}else{
				puts("\n<<Termina o guarda la partida actual primero>>");
			}
			break;
		case 8: //Leave
			// Liberar memoria antes de salir
			free(juego->Tablero);
			free(juego->secuencia);
			free(juego);
			free(jugadores);
			break;
		default:
			printf("\n-------------------ERROR-------------------");
			break;
			
		}
		
		if(juego->estado == 1){
		juego->estado = continuar_juego(juego,&OPC);
		system("cls");
			if(OPC == 8){
				printf("\nSaliendo del Programa...");
				free(juego);
				free(jugadores);
				free(juego->Tablero);
				free(juego->secuencia);
			}
		}
		
	} while(OPC !=8);
	
	return 0;
}

	/* FUNCIONES PRINCIPALES DEL PROGRAMA */

	void iniciar_juego(Juego *j,int esp){ /*ARRANCA EL JUEGO*/
		int i, f1, c1;
		for(i = 0; i < esp * esp; i++){			//Se genera la posicion aleatoria del Barco
			j->Tablero[i] = '.';
		}
		f1 = rand() % esp; 
		c1 = rand() % esp;
		j->Tablero[f1 * esp + c1] = 'B'; 
	}
	void mostrar_tablero(Juego *j, int esp){ /*MOSTRAR TABLERO*/
		int f, c;
		printf("\nTablero\n");
		for(f =0 ;f < esp; f++){
			for(c = 0;c < esp; c++){
				if(j->estado == 0 && j->Tablero[f * esp + c] == 'B'){ 
					printf(". ");
				}else{
					printf("%c ", j->Tablero[f * esp + c]);
				}
				printf(" ");
			}
			printf("\n");
		}
	}
	void realizar_disparos(Juego *j,int esp){ /* ARRANCAN LOS DISPAROS */
		int f,c;
		char *resultado;
		
		puts("\nIngrese las coordenadas a disparar ");
		do{
			printf("\nFila (1 - %d): ",esp);
			scanf("%d",&f);
			limpiarBuffer();
			f--; //usuario ingresa desde 1 hasta la ultima fila designada
			if(f<0 || f>=esp){
				puts("Error por desborde de mapa!!");
			}
		} while(f<0 || f>=esp);
		do{
			printf("\nColumna (1 - %d): ",esp);
			scanf("%d",&c);
			limpiarBuffer();
			c--; //usuario ingresa desde 1 hasta la ultima columna designada
			if(c<0 || c>=esp){
				puts("Error por desborde de mapa!!");
			}
		} while(c<0 || c>=esp);
		
		//Se guardan filas y memorias para la reasignacion
		j->secuencia = realloc(j->secuencia, (j->cant_tiros + 1) * sizeof *j->secuencia);
		if (j->secuencia == NULL) {
			puts("**ERROR al reservar secuencia**");
			exit(1);
		}
		
		j->secuencia[j->cant_tiros].fila = f+1;
		j->secuencia[j->cant_tiros].columna = c+1;
		j->cant_tiros++;
		
		if(j->Tablero[f * esp + c] == 'B'){ 
			puts("\n\t<<Atravesamos su cubierta!!Has Ganado el Juego!!>>");
			j->estado = 1; //Indicar de que gano el juego
			j->Tablero[f * esp + c] = 'X';
			resultado = "hundido";
			
		}else if(j->Tablero[f * esp + c] == 'A'){
			puts("\n\t<<Ya se disparo en esta coordenada>>...");
			resultado = "repetido";
			
		}else{
			puts("\n\t<<Solo es Agua mi capitan>>...");
			j->Tablero[f * esp + c]= 'A';
			resultado = "agua";
		}
	/* Guardado de historial de disparos en el archivo "disparos_%s.txt" */
		if(j->nombreJugador[0] != '\0'){
			guardar_historial_disparo(j->nombreJugador, f + 1, c + 1, resultado);
		}
	}
	void mostrar_ranking(struct record *jug, int posicion){ /* MOSTRAR RANKING */
		int i;
		if (posicion == 0 || jug == NULL) {
			printf("\nEl ranking esta vacio.\n");
			return;
		}
		
		printf("\n--- RANKING (menor cantidad de tiros = mejor) ---\n");
		printf("Pos  | Nombre                    | Tiros | Fecha y Hora\n");
		printf("----------------------------------------------------------\n");
	
		for (i = 0; i < posicion; i++) {
			printf("%-4d | %-25s | %-5d | %s\n", 
				   i + 1, jug[i].nombre, jug[i].puntaje, jug[i].fecha);
		}
	}
	void mostrar_secuencia_disparo(Juego *j){
		int i;
		if (j->cant_tiros == 0 || j->secuencia == NULL) {
			printf("\nLa secuencia esta vacia.\n");
			return;
		}
		for(i=0;i<j->cant_tiros;i++){
			printf("\n-->Disparo %d:",i+1);
			printf("\n 	*Fila: 		%d", j->secuencia[i].fila);
			printf("\n 	*Columna: 	%d", j->secuencia[i].columna);
		}
		printf("\n");
	}
	int continuar_juego(Juego *j,int *salida){
		int desicion;

		printf("\n¿Quieres continuar?\n");
		printf("1. Iniciar otra partida\n");
		printf("0. Salir\n");
		
		do{
			printf("\nEleccion: ");
			scanf("%d",&desicion);
			limpiarBuffer();
		} while(desicion != 0 && desicion != 1);
		
		if(desicion == 1){
			j->cant_tiros    = 0;
			j->juegoIniciado = 0;
			free(j->Tablero);   j->Tablero   = NULL;
			free(j->secuencia); j->secuencia = NULL;
			return 0;
			
		}else{
			*salida=8;
			return 0;
		}
	}
	void limpiarBuffer(){
		int c;
		while((c = getchar()) != '\n' && c != EOF);
	}
		
		/* FUNCIONES DE PERSISTENCIA DEL PROGRAMA */
	
	void cargar_ranking(int *total){
		char fechaLeida[30];
		int  extraidos, len;
		struct record *temporal;
		FILE *archivo = fopen("ranking.txt", "r");

		if(archivo == NULL){
			return;   /* Si no existe, no hay nada que cargar */
		}
		
		while(1){
			/* Reservar espacio preventivo para el siguiente registro */
			temporal = realloc(jugadores, (*total + 1) * sizeof(struct record));
			if(temporal == NULL){
				printf("Error: No se pudo asignar memoria al leer el ranking.\n");
				fclose(archivo);
				return;
			}
			jugadores = temporal;
			
			/*
			* Mascara de formato:
			* [%29[^]]]  -> Lee la fecha hasta el corchete de cierre ']'
			* ] Jugador: -> Consume el texto "] Jugador: "
			* %49[^|]    -> Lee el nombre hasta el '|'
			* | Puntaje: -> Consume " | Puntaje: "
			* %d         -> Lee el puntaje entero
			*/
			extraidos = fscanf(archivo,
							   "[%29[^]]] Jugador: %49[^|] | Puntaje: %d\n",
							   fechaLeida,
							   jugadores[*total].nombre,
							   &jugadores[*total].puntaje);
			
			if(extraidos == EOF) {
				break;
			}
			
			if(extraidos == 3){
				/* Limpiar espacio residual al final del nombre (antes del '|') */
				len = strlen(jugadores[*total].nombre);
				while(len > 0 && jugadores[*total].nombre[len - 1] == ' '){
					jugadores[*total].nombre[len - 1] = '\0';
					len--;
				}
				/* Guardar la fecha leida */
				strcpy(jugadores[*total].fecha, fechaLeida);
				
				(*total)++;
			}else{
				/* Linea con formato invalido: saltarla */
				fscanf(archivo, "%*[^\n]\n");
			}
		}
		
		fclose(archivo);
		
		/* Ajustar al tamaño real (realloc reservo un espacio de mas) */
		if(*total > 0){
			jugadores = realloc(jugadores, (*total) * sizeof(struct record));
		}else{
			free(jugadores);
			jugadores = NULL;
		}
		
		printf("-> Se cargaron %d jugadores desde ranking.txt\n", *total);
	}
	void mostrar_mejor_jugador(int total){
		int i, mejor_indice, leidos;
		
		if(total == 0 || jugadores == NULL){
			printf("  [Aun no hay record]\n\n");
			return;
		}
		
		mejor_indice = 0;
		for(i = 1; i < total; i++){
			if(jugadores[i].puntaje < jugadores[mejor_indice].puntaje){
				mejor_indice = i;
			}
		}
		
		printf("  *** RECORD: %s con %d tiros", jugadores[mejor_indice].nombre, jugadores[mejor_indice].puntaje);
		
		/* Calcular tiempo transcurrido a partir de la fecha guardada */
		if(jugadores[mejor_indice].fecha[0] != '\0'){
			struct tm tmRecord = {0};											//estructura del <time.h> que representa una fecha/hora descompuesta en campos: día, mes, año, hora, minutos, segundos, etc
			leidos = sscanf(jugadores[mejor_indice].fecha,						//sscanf es igual a fscanf pero en vez de leer de un archivo, lee de un string
								"%d/%d/%d - %d:%d:%d",
								&tmRecord.tm_mday,
								&tmRecord.tm_mon,
								&tmRecord.tm_year,
								&tmRecord.tm_hour,
								&tmRecord.tm_min,
								&tmRecord.tm_sec);
			
			if(leidos == 6){
				tmRecord.tm_year -= 1900; //tm_year no espera el año completo (2026), sino años desde 1900. Hay que restarle 1900 para que quede 126, que es el valor que struct tm espera
				tmRecord.tm_mon  -= 1; //tm_mon va de 0 (enero) a 11 (diciembre), no de 1 a 12
				tmRecord.tm_isdst = -1; //Es el flag de horario de verano ("daylight saving time"). Con -1 le decimos al sistema "no sé si aplica horario de verano en esta fecha, calculalo vos".
				
				time_t tRecord = mktime(&tmRecord); //mktime toma esa struct tm (fecha descompuesta en campos) y la convierte a time_t, que es un número 
				time_t tAhora  = time(NULL); //te da el número correspondiente a "ahora mismo".
				double diff    = difftime(tAhora, tRecord); //difftime simplemente hace tAhora - tRecord y te da la diferencia en segundos
				
				if(diff < 60)
					printf(" (hace %.0f segundos)", diff);
				else if(diff < 3600)
					printf(" (hace %.0f minutos)", diff / 60.0);
				else if(diff < 86400)
					printf(" (hace %.1f horas)", diff / 3600.0);
				else
					printf(" (hace %.1f dias)", diff / 86400.0);
			}
		}
		printf(" ***\n\n");
	}
	void guardar_ranking(char *nombre, int puntaje){
		time_t tiempoActual;
		struct tm *infoTiempo;
		char cadenaFecha[30];
		
		FILE *archivo = fopen("ranking.txt", "a");
		if(archivo == NULL){
			puts("No se pudo abrir ranking.txt para escritura");
			return;
		}
		
		time(&tiempoActual);
		infoTiempo = localtime(&tiempoActual);
		strftime(cadenaFecha, sizeof(cadenaFecha), "%d/%m/%Y - %H:%M:%S", infoTiempo);
		
		fprintf(archivo, "[%s] Jugador: %s | Puntaje: %d\n", cadenaFecha, nombre, puntaje);
		
		fclose(archivo);
		puts(">> Ranking guardado en ranking.txt");
	}
	void guardar_historial_disparo(char *nombre, int fila, int columna, char *resultado){
		char nombreArchivo[80];
		sprintf(nombreArchivo, "disparos_%s.txt", nombre); //Escribe el resultado dentro de un string (el primer parámetro, nombreArchivo)
											/*Si nombre fuera "Maria", sería "disparos_Maria.txt". 
											Así cada jugador termina teniendo su propio archivo de historial, sin mezclarse con los de otros.*/
		FILE *archivo = fopen(nombreArchivo, "a");
		if(archivo == NULL){
			printf("No se pudo abrir %s\n", nombreArchivo);
			return;
		}
		
		fprintf(archivo, "%d,%d,%s\n", fila, columna, resultado);
		fclose(archivo);
	}
	void guardar_partida(Juego *j){ 
		/* Solo puede haber una partida guardada a la vez */
		int i;
		int esp = j->esp;
		
		FILE *archivo = fopen("partida_guardada.txt", "w");
		if(archivo == NULL){
			puts("Error al abrir partida_guardada.txt");
			return;
		}
		
		fprintf(archivo, "%s\n",  j->nombreJugador);
		fprintf(archivo, "%d\n",  esp);
		fprintf(archivo, "%d\n",  j->cant_tiros);
		
		/* Tablero como cadena continua */
		for(i = 0; i < esp * esp; i++){
			fputc(j->Tablero[i], archivo);
		}
		fputc('\n', archivo);
		
		/* Secuencia de disparos */
		for(i = 0; i < j->cant_tiros; i++){
			fprintf(archivo, "%d,%d\n", j->secuencia[i].fila, j->secuencia[i].columna);
		}
		
		fclose(archivo);
		puts(">> Partida guardada en partida_guardada.txt");
	}
	int cargar_partida(Juego *j){
		int  esp, tiros, i;
		char linea[1024]; //linea es un buffer que lee cada línea del archivo, una por una, con fgets
		int  fi, co;
		
		FILE *archivo = fopen("partida_guardada.txt", "r");
		if(archivo == NULL){
			puts("No existe partida_guardada.txt");
			return 0;
		}
		
		/* Nombre del jugador */
		if(fgets(linea, sizeof(linea), archivo) == NULL){ //fgets lee una línea completa del archivo (hasta el \n inclusive) y la guarda en linea
			fclose(archivo); 
			return 0; 
		}
		linea[strcspn(linea, "\n")] = '\0'; //busca en qué posición está el \n dentro del string, y esa posición se reemplaza por '\0'
		strcpy(j->nombreJugador, linea);
		
		/* Tamaño del tablero */
		if(fgets(linea, sizeof(linea), archivo) == NULL){
			fclose(archivo); 
			return 0; 
		}
		esp = atoi(linea); //la convierte de texto a entero
		if(esp <= 0){
			fclose(archivo); 
			return 0; 
		}
		
		/* Cantidad de tiros */
		if(fgets(linea, sizeof(linea), archivo) == NULL){
			fclose(archivo); 
			return 0; 
		}
		tiros = atoi(linea); 
		
		/* Tablero */
		free(j->Tablero);
		j->Tablero = malloc(esp * esp * sizeof(char));
		if(j->Tablero == NULL){
			fclose(archivo); return 0; 
		}
		
		if(fgets(linea, sizeof(linea), archivo) == NULL){
			fclose(archivo); return 0; 
		}
		for(i = 0; i < esp * esp; i++){
			j->Tablero[i] = linea[i];
		}
		
		/* Secuencia de disparos */
		free(j->secuencia);
		j->secuencia  = NULL;
		j->cant_tiros = 0;
		
		for(i = 0; i < tiros; i++){
			if(fgets(linea, sizeof(linea), archivo) == NULL){
				break;
			}
			
			j->secuencia = realloc(j->secuencia, (j->cant_tiros + 1) * sizeof(*j->secuencia));
			if(j->secuencia == NULL){
				fclose(archivo); return 0; 
			}
			
			sscanf(linea, "%d,%d", &fi, &co); //extrae los dos números separados por coma y los guarda en fi y co
			j->secuencia[j->cant_tiros].fila    = fi;
			j->secuencia[j->cant_tiros].columna = co;
			j->cant_tiros++;
		}
		
		fclose(archivo);
		
		j->esp        = esp;
		j->juegoIniciado  = 1;
		j->estado         = 0;
		
		return 1;
	}
