#ifndef ORIGINE_DEBUG_H
#define ORIGINE_DEBUG_H

#ifndef ORiGiNe_DEBUG

#define ORIGINE_DEBUG_BEGIN() { }
#define ORIGINE_DEBUG_STDOUT(str, ...) { }
#define ORIGINE_DEBUG_STDOUT_ITER(origine_debug_tab, origine_debug_size) { }

#else

#include <stdio.h>

/*!
 *	\def ORIGINE_DEBUG_BEGIN
 *	DEBUG : Affiche une phrase d'intro avec le nom du fichier, et la fonction
 */
#define ORIGINE_DEBUG_BEGIN() printf("DEBUG : %s (%s, line : %d) :\n",__FUNCTION__,__BASE_FILE__,__LINE__)
/*!
 *	\def ORIGINE_DEBUG_STDOUT
 *	\param str String a print
 *	DEBUG : Affichage d'une string
 */
#define ORIGINE_DEBUG_STDOUT(str, ...) { \
	ORIGINE_DEBUG_BEGIN() ; \
	printf(str, ##__VA_ARGS__); \
}

/*!
 *	\def ORIGINE_DEBUG_STDOUT_ITER
 *	\param tab Tableau à afficher
 *	\param size Taille du tableau
 *	DEBUG : Affichage d'une string
 */
#define ORIGINE_DEBUG_STDOUT_ITER(origine_debug_tab, origine_debug_size) { \
	int origine_debug_i; \
	for ( origine_debug_i = 0 ; origine_debug_i < origine_debug_size-1 ; origine_debug_i++) \
		printf("%d-", origine_debug_tab[origine_debug_i]); \
	printf("%d\n",origine_debug_tab[origine_debug_size-1]);\
}

/*!
 *	@def ORIGINE_DEBUG_POISON
 *	@param nbr Nombre de données poisons
 *	@param data Data a écrire
 *	Ecrit des données poisons sur la liaison série
 */
#define ORIGINE_DEBUG_POISON(data,nbr) { \
	int origine_debug_i; \
	for (origine_debug_i = 0 ; origine_debug_i < nbr ; origine_debug_i++) \
		write(device,data,i); \
}

#endif /* ORiGiNe_DEBUG */
// FIXME: la suite à remettre là haut avec un makefile joli pour up sur arduino
/*!
 *	@def ORIGINE_DEBUG_ARD_LED
 *	@param time Le temps d'attente en ms
 *	Allume la LED 13 pendant le temps indiqué, puis l'éteint
 */
#define ORIGINE_DEBUG_ARD_LED(time) { \
	digitalWrite(13,HIGH); \
	delay(time); \
	digitalWrite(13,LOW); \
}

/*!
 *	@def ORIGINE_DEBUG_ARD_POISON
 *	@param nbr Nombre de données poisons
 *	@param data Data a écrire
 *	Ecrit des données poisons sur la liaison série
 */
#define ORIGINE_DEBUG_ARD_POISON(data,nbr) { \
	int origine_debug_i; \
	for (origine_debug_i = 0 ; origine_debug_i < nbr ; origine_debug_i++) \
		Serial.write(data); \
}
#endif /* ORIGINE_DEBUG_H */
