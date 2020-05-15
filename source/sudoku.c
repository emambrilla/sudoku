#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

typedef struct{
	short			values[9];
} sudoku_cell_option_type;

typedef struct{
	sudoku_cell_option_type	*options;	
	short			value;
} sudoku_cell_type;

typedef struct{
	sudoku_cell_type	cells[9][9];
} sudoku_type;

int sudoku_load( FILE *file, sudoku_type *sudoku );
int sudoku_fprint( FILE *file, const sudoku_type *sudoku );
int sudoku_copy( const sudoku_type *orig, sudoku_type *dest, 
	short row, short column, short value );
int sudoku_destroy( sudoku_type *sudoku );
int sudoku_reduce( sudoku_type *sudoku );
int sudoku_nvalues( const sudoku_type *sudoku );
int sudoku_chk( const sudoku_type *sudoku );
int sudoku_fix( sudoku_type *sudoku );
int sudoku_simplify( sudoku_type *sudoku, short row, short column );

int main( int argc, char *argv[] ){

	sudoku_type	sudoku;
	short	last_nvalues=0,	now_nvalues;

	if ( sudoku_load( stdin, &sudoku ) ) goto return_error;
	if ( sudoku_fix( &sudoku ) ) goto return_error;
/*
	while ( ( now_nvalues=sudoku_nvalues( &sudoku ) ) < 81 ){
		if ( last_nvalues == now_nvalues ){
			// Fuerza bruta
			if ( sudoku_fix( &sudoku ) ) goto return_error;
			break;
		} // if
		else{
			if ( sudoku_reduce( &sudoku ) ) goto return_error;
			if ( sudoku_simplify( &sudoku, 0, 0 ) ) goto return_error;
		} // else
		last_nvalues = now_nvalues;
	} // while
*/
	if ( sudoku_chk( &sudoku ) ){
		fprintf( stderr, "%s: error chequeo\n", argv[0] );
		goto return_error;
	} // if
	if ( sudoku_fprint( stdout, &sudoku ) ) goto return_error;

	sudoku_destroy( &sudoku );

	return( 0 );

return_error:
	fprintf( stderr, "%s: error main\n", argv[0] );
	return( -1 );

} // main

int sudoku_simplify( sudoku_type *sudoku, short row, short column ){

	int	i, j, k, l, m;
	int simplify_flag=0;

	for ( i=1; i<=9; i++ ){
			for ( j=1; j<=9; j++ ){
					if ( ( i < row ) && ( j <= column ) ) continue;
					if ( !sudoku->cells[i-1][j-1].options ) continue;
					// misma fila
					for ( k=1; k<=9; k++ ){
						if ( k == j ) continue;
						if ( !sudoku->cells[i-1][k-1].options )
							sudoku->cells[i-1][j-1].options->values
								[sudoku->cells[i-1][k-1].value-1] = 0;
					} // for
					// misma columna
					for ( k=1; k<=9; k++ ){
						if ( k == i ) continue;
						if ( !sudoku->cells[k-1][j-1].options )
							sudoku->cells[i-1][j-1].options->values
								[sudoku->cells[k-1][j-1].value-1] = 0;
					} // for
					// misma cuadricula
					for ( k=(((i-1)/3)*3)+1;  k<=(((i-1)/3)*3)+3; k++ ){
						for ( l=(((j-1)/3)*3)+1;  l<=(((j-1)/3)*3)+3; l++ ){
							if ( ( k == i ) && ( l == j ) ) continue;
							if ( !sudoku->cells[k-1][l-1].options )
								sudoku->cells[i-1][j-1].options->values
									[sudoku->cells[k-1][l-1].value-1] = 0;
						} // for
					} // for
					for ( k=1, l=0;  k<=9; k++ ){
						if ( sudoku->cells[i-1][j-1].options->values[k-1] ){ l++; m=k; }
					} // for
					if ( l == 1 ){
						free ( sudoku->cells[i-1][j-1].options );
					  sudoku->cells[i-1][j-1].options=NULL;
						sudoku->cells[i-1][j-1].value=m;
						simplify_flag=1;
					} // if
					else if ( l == 0 )
						goto return_nok;
			} // for
	} //for

	if ( simplify_flag ){
		return ( sudoku_simplify( sudoku, row, column ) );
	} // if

	return( 0 );

return_error:
	fprintf( stderr, " error sudoku_simplify\n" );
	return( -1 );

return_nok:
	return( 1 );

} // sudoku_simplify

int sudoku_load( FILE *file, sudoku_type *sudoku ){

	char	caux[1024];
	int	i, j, k;

	for ( i=1; i<=9; i++ ){
		if ( !fgets( caux, sizeof( caux ), file ) ) goto return_error;
		if ( strlen( caux ) < 9 ) goto return_error;
		for ( j=1; j<=9; j++ ){
			switch( caux[j-1] ){
				case	'1':
				case	'2':
				case	'3':
				case	'4':
				case	'5':
				case	'6':
				case	'7':
				case	'8':
				case	'9':
					sudoku->cells[i-1][j-1].value=(short)caux[j-1] - (short)'1'+1;
					sudoku->cells[i-1][j-1].options=NULL;
					break;
				default:
					sudoku->cells[i-1][j-1].value=0;
					if ( !( sudoku->cells[i-1][j-1].options=
										malloc( sizeof( sudoku_cell_option_type ) ) ) )
										goto return_error;
					for ( k=1; k<=9; k++ ) sudoku->cells[i-1][j-1].options->values[k-1]=1;
					break;
			} // switch
		} // for
	} // for

	if ( sudoku_simplify( sudoku, 0, 0 ) ) goto return_error;
	return( 0 );

return_error:
	fprintf( stderr, " error sudoku_load\n" );
	return( -1 );

} // sudoku_load

int sudoku_fprint( FILE *file, const sudoku_type *sudoku ){

	int	i, j, k;

	fprintf( file, "\n+-------+-------+-------+\n" );
	for( i=1; i<=9; i++ ){
		fprintf( stdout, "| " );
		for( j=1; j<=9; j++ ){
			if ( !sudoku->cells[i-1][j-1].options )
				fprintf( file, "%d", sudoku->cells[i-1][j-1].value );
			else{
				for( k=1; k<=9; k++ ){
					if ( sudoku->cells[i-1][j-1].options->values[k-1] )
								fprintf( file, "%d", k );
				} // for
			} // else
			if (  j % 3 ) fprintf( file, " " );
			if ( !( j %3 ) && ( j != 9 ) ) fprintf( file, " | " );
		} // for
		fprintf( file, " |\n" );
		if ( !( i %3 ) && ( i != 9 ) ) 
			fprintf( file, "|-------+-------+-------|\n" );
	} // for
	fprintf( file, "+-------+-------+-------+\n" );
	

	return( 0 );

return_error:
	fprintf( stderr, " error sudoku_fprint\n" );
	return( -1 );

} // sudoku_fprint

int sudoku_copy( const sudoku_type *orig, sudoku_type *dest, 
	short row, short column, short value ){

	int i, j;

	for( i=1; i<=9; i++ ){
					for( j=1; j<=9; j++ ){
						if ( ( i == row ) && ( j == column ) ){
										dest->cells[i-1][j-1].value=value;
										dest->cells[i-1][j-1].options = NULL;
						} // if
						else{
										dest->cells[i-1][j-1].value=
																	orig->cells[i-1][j-1].value;
										if ( orig->cells[i-1][j-1].options ){
													dest->cells[i-1][j-1].options= 
														malloc( sizeof( sudoku_cell_option_type ) );
													memcpy( dest->cells[i-1][j-1].options, 
														orig->cells[i-1][j-1].options, 
														sizeof( sudoku_cell_option_type ) );
										} // if
										else dest->cells[i-1][j-1].options = NULL;
						} // else
					} // for
	} // for

	return( 0 );

} // sudoku_copy

int sudoku_destroy( sudoku_type *sudoku ){

	int i, j;

	for( i=1; i<=9; i++ ){
					for( j=1; j<=9; j++ ){
						sudoku->cells[i-1][j-1].value=0;
						if ( sudoku->cells[i-1][j-1].options ){
									free( sudoku->cells[i-1][j-1].options );
									sudoku->cells[i-1][j-1].options= NULL;
						} // if
					} // for
	} // for

	return( 0 );

} // sudoku_destroy

int sudoku_reduce( sudoku_type *sudoku ){

	int	i, j, k, l, m;
	sudoku_type	sudoku_ws;

	for( i=1; i<=9; i++ ){
		for( j=1; j<=9; j++ ){
			if ( !sudoku->cells[i-1][j-1].options ) continue;
			for( k=1; k<=9; k++ ){
				if ( !sudoku->cells[i-1][j-1].options->values[k-1] ) continue;
				// Clono
				if ( sudoku_copy( sudoku, &sudoku_ws, i, j, k ) ) goto return_error;
				switch( sudoku_simplify( &sudoku_ws, 0, 0 ) ){
				case 0:
					break;
				case 1:
//	fprintf( stdout, "-> Inconsistencia (%d,%d): %d\n", i, j, k );
					sudoku->cells[i-1][j-1].options->values[k-1]=0;
					break;
				default:
					goto return_error;
					break;
				} // switch
				// Libero
				sudoku_destroy( &sudoku_ws );
			} // for
			for( k=1, l=0; k<=9; k++ ){
				if ( sudoku->cells[i-1][j-1].options->values[k-1] ){ l++, m=k; }
			} // for
			if ( l == 0 ) goto return_error;
			else if ( l == 1 ){
				free ( sudoku->cells[i-1][j-1].options );
				sudoku->cells[i-1][j-1].options=NULL;
				sudoku->cells[i-1][j-1].value=m;
//				fprintf( stdout, "-> Cazado (%d,%d)= %d\n", i, j, m );
			} // else if
		} // for
	} // for

	return( 0 );

return_error:
	return( -1 );

} // sudoku_reduce

int sudoku_nvalues( const sudoku_type *sudoku ){

	int	i, j, k=0;

	for( i=1; i<=9; i++ ){
		for( j=1; j<=9; j++ ){
			if ( !sudoku->cells[i-1][j-1].options ) k++;
		} // for
	} // for

	return( k );

} // sudoku_reduce

int sudoku_chk( const sudoku_type *sudoku ){

	int	i, j, a, b, k=0;

	short	val[9];

	// Por filas
	for( i=1; i<=9; i++ ){
		for( j=1; j<=9; j++ ) val[j-1]=0;
		for( j=1; j<=9; j++ ){
			if ( sudoku->cells[i-1][j-1].options ) goto return_error;
			val[sudoku->cells[i-1][j-1].value-1]++;
		} // for
		for( j=1; j<=9; j++ ){
			if ( val[j-1] != 1 ) goto return_error;
		} // for
	} // for

	// Por columnas
	for( j=1; j<=9; j++ ){
		for( i=1; i<=9; i++ ) val[i-1]=0;
		for( i=1; i<=9; i++ ){
			if ( sudoku->cells[i-1][j-1].options ) goto return_error;
			val[sudoku->cells[i-1][j-1].value-1]++;
		} // for
		for( i=1; i<=9; i++ ){
			if ( val[i-1] != 1 ) goto return_error;
		} // for
	} // for

	for( a=0; a<=2; a++ ){
		for( b=0; b<=2; b++ ){
					for( i=1; i<=9; i++ ) val[i-1]=0;
					for( i=(a*3)+1; i<=(a*3)+3; i++ ){
						for( j=(b*3)+1; j<=(b*3)+3; j++ ){
							val[sudoku->cells[i-1][j-1].value-1]++;
						} // for
					} // for
		} // for
		for( i=1; i<=9; i++ ){
			if ( val[i-1] != 1 ) goto return_error;
		} // for
	} // for

	return( 0 );

return_error:
	return( -1 );

} // sudoku_reduce

int sudoku_fix( sudoku_type *sudoku ){


	int	i, j, k;
	sudoku_type	sudoku_ws;



	for( i=1; i<=9; i++ ){
		for( j=1; j<=9; j++ ){
			if ( !sudoku->cells[i-1][j-1].options ) continue;
			for( k=1; k<=9; k++ ){
				if ( sudoku->cells[i-1][j-1].options->values[k-1] ){
					if ( sudoku_copy( sudoku, &sudoku_ws, i, j, k ) )
						goto return_error;
					switch( sudoku_simplify( &sudoku_ws, i, j ) ){
					case 0:
									if ( sudoku_nvalues( &sudoku_ws ) == 81 ){
											if ( !sudoku_chk( &sudoku_ws ) ){
												// Encontrado
												sudoku_destroy( sudoku );
												sudoku_copy( &sudoku_ws, sudoku, 0, 0, 0 );
												sudoku_destroy( &sudoku_ws );
												goto return_ok;
											} // if
									} // if
									else{
											if ( !sudoku_fix( &sudoku_ws ) ){
												sudoku_destroy( sudoku );
												sudoku_copy( &sudoku_ws, sudoku, 0, 0, 0 );
												sudoku_destroy( &sudoku_ws );
												goto return_ok;
											} // if
									} // else
						break;
					case 1:
						break;
					default:
						goto return_error;
						break;
					} // switch
					sudoku_destroy( &sudoku_ws );
				} //
			} // for
			goto return_nok;
		} // for
	} // for

return_nok:
	return( 1 );

return_error:
	return( -1 );

return_ok:
	return( 0 );

} // sudoku_fix
