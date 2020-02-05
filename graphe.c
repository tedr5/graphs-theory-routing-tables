#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define n 100
#define TIER1 -500
#define TIER2 -499
#define TIER3 -498
#define INFINI 1000

//structure du graphe
typedef struct
{
  int nb_sommets;		//nombre de sommets
  int **matrice_adj;		//matrice adjacence
  int **temps;			// matrice des temps
  int *couleur;			//tableau pour parcours profondeur
  int depart;			//depart du routage
  int destination;		//destination du routage
  int **P;			//matrice des predecesseurs
  int **W;			//matrice modifiée pour Floyd Warshall
} graphe;

typedef struct Element Element;
struct Element
{
  int nombre;
  Element *suivant;
};

typedef struct Pile Pile;
struct Pile
{
  Element *premier;
};

Pile * initialiser ()
{
  Pile *pile = malloc (sizeof (*pile));
  pile->premier = NULL;
  return pile;
}

void empiler (Pile * pile, int nvNombre)
{
  Element *nouveau = malloc (sizeof (*nouveau));
  if (pile == NULL || nouveau == NULL)
    {
      exit (EXIT_FAILURE);
    }

  nouveau->nombre = nvNombre;
  nouveau->suivant = pile->premier;
  pile->premier = nouveau;
}

int depiler (Pile * pile)
{
  if (pile == NULL)
    {
      exit (EXIT_FAILURE);
    }

  int nombreDepile = 0;
  Element *elementDepile = pile->premier;

  if (pile != NULL && pile->premier != NULL)
    {
      nombreDepile = elementDepile->nombre;
      pile->premier = elementDepile->suivant;
      free (elementDepile);
    }

  return nombreDepile;
}

Pile * viderPile (Pile * pile)
{
  while (pile->premier != NULL)
    {
      depiler (pile);
    }
  return pile;
}

int nb_aleatoire (int min, int max)
{
  int nb = (rand () % (max - min + 1)) + min;
  return nb;
}

int alea_temps (int type)
{
  int temps;

  switch (type)
    {
    case TIER1:
      temps = nb_aleatoire (5, 10);
      return temps;

    case TIER2:
      temps = nb_aleatoire (10, 20);
      return temps;

    case TIER3:
      temps = nb_aleatoire (15, 50);
      return temps;
    }
  return 0;
}

//Crée le fichier "graphX.png" (X = str en param)
//Le fichier n'est généré que si la fonction init_graph est implémentée
void ecrire_fichier_dot (graphe G, char *str)
{
  if (G.matrice_adj == NULL)
    return;
  FILE *f;
  int i, j;
  char buf[1000];
  sprintf (buf, "graph%s.dot", str);

  if (!(f = fopen (buf, "w")))
    {
      perror ("Opening dot file failure\n");
      exit (2);
    }

  fprintf (f, "graph G { \n");

  fprintf (f,
	   "graph[splines=true, overlap=false];  ranksep = .100; nodesep=1000.5;\n edge[len= 100,weight=1,color = navyblue,concentrate=false]; node[color=green,shape=polygon,style = filled,fontsize=11,fontcolor=black];\n");
  for (i = 0; i < G.nb_sommets; i++)
    {

      if (i < 7)
	{
	  fprintf (f,
		   "%d node[color=green,shape=polygon,style = filled,fontsize=11,fontcolor=black];\n",
		   i);
	}
      else if (i == 8 || i < 27)
	{
	  fprintf (f,
		   "%d node[color=yellow,shape=triangle,style = filled,fontsize=11,fontcolor=black];\n",
		   i);
	}
      else if (i == 28 || i < 100)
	{
	  fprintf (f,
		   "%d node[color=red,shape=circle,style = filled,fontsize=11,fontcolor=black];\n",
		   i);
	}
    }
  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < i; j++)
	{
	  if (G.matrice_adj[i][j] != 0)
	    {
	      fprintf (f, "%d -- %d [label=%d,fontcolor=red];  \n", i, j,
		       G.temps[i][j]);
	    }
	}

    }
  fprintf (f, "} \n");
  fclose (f);

  sprintf (buf, "sfdp -x -Goverlap=scale  -Tpng graph%s.dot -o Network%s.png",
	   str, str);
  system (buf);

}

void ecrire_fichier_dot2 (graphe G, char *str)
{
  if (G.matrice_adj == NULL)
    return;
  FILE *f;
  int i, j;
  char buf[100];
  sprintf (buf, "graph%s.dot", str);

  if (!(f = fopen (buf, "w")))
    {
      perror ("Opening dot file failure\n");
      exit (2);
    }

  fprintf (f, "graph G { \n");

  fprintf (f,
	   "graph[splines=true, overlap=false ]; nodesep=5.5;\n edge[len= 100,weight=1,color = navyblue,concentrate=false]; node[color=green,shape=circle,style = filled,fontsize=11,fontcolor=black];\n");
  for (i = 0; i < G.nb_sommets; i++)
    {
      fprintf (f, "%d\n", i);
    }
  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < i; j++)
	{
	  if (G.matrice_adj[i][j] != 0)
	    {
	      fprintf (f, "%d -- %d [label=%d,fontcolor=red];  \n", i, j,
		       G.temps[i][j]);
	    }
	}

    }
  fprintf (f, "} \n");
  fclose (f);

  sprintf (buf, "neato -Tpng graph%s.dot -o Network%s.png", str, str);
  system (buf);

}

graphe init_graphe (graphe G)
{
  G.nb_sommets = n;
  G.couleur = calloc (G.nb_sommets, sizeof (int));
  int i;
  G.matrice_adj = calloc (G.nb_sommets, sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.matrice_adj[i] = calloc (G.nb_sommets, sizeof (int));
    }

  G.temps = calloc (G.nb_sommets, sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.temps[i] = calloc (G.nb_sommets, sizeof (int));
    }


  G.P = malloc (G.nb_sommets * sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.P[i] = malloc (G.nb_sommets * sizeof (int));
    }


  G.W = malloc (G.nb_sommets * sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.W[i] = malloc (G.nb_sommets * sizeof (int));
    }
  return G;
}

graphe alea_graphe (graphe G)
{
  G = init_graphe (G);
  int i, j;
  /*TIER1 */ for (i = 0; i <= 7; i++)
    {
      for (j = 0; j <= 7; j++)
	{
	  if ((rand () % 100) > 25 && i != j)
	    {
	      G.matrice_adj[i][j] = G.matrice_adj[j][i] = 1;
	      G.temps[j][i] = G.temps[i][j] = alea_temps (TIER1);
	    }
	}
    }
  /*TIER2 */
  int nb, nb2, x;
  for (i = 8; i <= 27; i++)
    {				//pour chaque noeud du TIER2 
      nb = nb_aleatoire (1, 2);	// 1 ou 2 noeuds de TIER1 
      for (j = 0; j < nb; j++)
	{
	  x = nb_aleatoire (0, 7);	//choix du noeud du TIER1 
	  G.matrice_adj[i][x] = G.matrice_adj[x][i] = 1;
	  G.temps[i][x] = G.temps[x][i] = alea_temps (TIER2);
	}
    }

  for (i = 8; i <= 27; i++)
    {				//pour chaque noeud du TIER2
      nb2 = nb_aleatoire (2, 3);	// 2 ou 3 noeuds de TIER2
      for (j = 0; j < nb2; j++)
	{
	  x = nb_aleatoire (8, 27);	//choix du noeud de TIER2   
	  G.matrice_adj[i][x] = G.matrice_adj[x][i] = 1;
	  G.temps[i][x] = G.temps[x][i] = alea_temps (TIER2);
	}
    }
  /*TIER3 */
  for (i = 28; i <= 99; i++)
    {
      for (j = 0; j < 2; j++)
	{
	  x = nb_aleatoire (8, 27);	// 2 noeuds de TIER2
	  G.matrice_adj[i][x] = G.matrice_adj[x][i] = 1;
	  G.temps[i][x] = G.temps[x][i] = alea_temps (TIER3);
	}
    }
  for (i = 28; i <= 99; i++)
    {
      for (j = 0; j < 1; j++)
	{
	  x = nb_aleatoire (28, 99);	//1 noeud du TIER3
	  G.matrice_adj[i][x] = G.matrice_adj[x][i] = 1;
	  G.temps[i][x] = G.temps[x][i] = alea_temps (TIER3);

	}
    }
  return G;
}

graphe colorier (graphe G, int sommet, int couleur)
{
  int i;
  printf ("Noeud colorié: %d\n", sommet);
  G.couleur[sommet] = couleur;
  for (i = 0; i < G.nb_sommets; i++)
    {
      if (G.couleur[i] == 0 && G.couleur[i] != couleur
	  && G.matrice_adj[i][sommet] != 0 && G.matrice_adj[sommet][i] != 0)
	{
	  G = colorier (G, i, couleur);
	}
    }
  return G;
}

graphe parcours_profondeur (graphe G)
{
  int i, j, couleur, cpt = 0;
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.couleur[i] = 0;
    }
  couleur = 1;
  for (j = 0; j < G.nb_sommets; j++)
    {
      if (G.couleur[j] == 0)
	{
	  G = colorier (G, j, couleur);
	  couleur++;
	}
    }
  for (i = 0; i < G.nb_sommets; i++)
    {
      printf ("%d ", G.couleur[i]);
      if (G.couleur[i] == 1)
	{
	  cpt++;
	}
    }
  printf ("\n");

  if (cpt == G.nb_sommets)
    {
      printf ("Le réseau est bien connexe.\n");
    }
  else
    {
      printf
	("Le réseau n'est pas connexe, création d'un nouveau réseau.\n");
      G = alea_graphe (G);
    }
  return G;
}

void libere_graphe (graphe G)
{
  int i;
  for (i = 0; i < G.nb_sommets; i++)
    {
      free (G.matrice_adj[i]);
    }
  free (G.matrice_adj);

  for (i = 0; i < G.nb_sommets; i++)
    {
      free (G.temps[i]);
    }
  free (G.temps);

  for (i = 0; i < G.nb_sommets; i++)
    {
      free (G.P[i]);
    }
  free (G.P);

  for (i = 0; i < G.nb_sommets; i++)
    {
      free (G.W[i]);
    }
  free (G.W);

  free (G.couleur);

}

//Afficher temps pour chaque aretes
void auxiliaire (graphe G)
{
  FILE *f = fopen ("temps.txt", "w");
  if (f == NULL)
    {
      printf ("OPEN ERROR\n");
      exit (-1);
    }
  int i, j;
  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < G.nb_sommets; j++)
	{
	  if (G.temps[i][j] != 0)
	    {
	      fprintf (f, "%d ", i);
	      fprintf (f, "%d ", j);
	      fprintf (f, "%d \n", G.temps[i][j]);
	    }
	}
    }
  fclose (f);
}

graphe alea_graphe_test (int nb, graphe G)
{

  G.nb_sommets = nb;

  int i, j;
  G.matrice_adj = calloc (G.nb_sommets, sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.matrice_adj[i] = calloc (G.nb_sommets, sizeof (int));
    }

  G.temps = calloc (G.nb_sommets, sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.temps[i] = calloc (G.nb_sommets, sizeof (int));
    }


  G.P = malloc (G.nb_sommets * sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.P[i] = malloc (G.nb_sommets * sizeof (int));
    }


  G.W = malloc (G.nb_sommets * sizeof (int *));
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.W[i] = malloc (G.nb_sommets * sizeof (int));
    }

  G.couleur = malloc (G.nb_sommets * sizeof (int));

  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < G.nb_sommets; j++)
	{
	  if (rand () % 100 > 80 && i != j)
	    {
	      G.matrice_adj[i][j] = G.matrice_adj[j][i] = 1;
	      G.temps[i][j] = G.temps[j][i] = alea_temps (TIER1);
	    }
	}
    }
  return G;
}

void afficherPile (Pile * pile, graphe G)
{
  if (pile == NULL)
    {
      exit (EXIT_FAILURE);
    }
  Element *noeud = pile->premier;
  do
    {
      printf ("Le message prend le noeud %d\n", noeud->nombre);
      noeud = noeud->suivant;
    }
  while (noeud != NULL);
}

void chemin_entre_deux (graphe G)
{
  printf ("Noeud émetteur:");
  scanf ("%d", &(G.depart));
  printf ("\n");
  printf ("Noeud destinataire:");
  scanf ("%d", &(G.destination));
  printf ("\n");
  int receveur = G.destination;
  Pile *p = initialiser ();
  printf ("Le message part du noeud %d\n", G.depart);
  do
    {
      G.destination = G.P[G.depart][G.destination];
      //~ printf("EMPILER:%d\n",G.destination);
      empiler (p, G.destination);
    }
  while (G.P[G.depart][G.destination] != G.depart);
  afficherPile (p, G);

  printf ("Le message arrive au noeud %d\n", receveur);
  printf ("Le temps de réception du message de %d à %d est de :%d\n",
	  G.depart, receveur, G.W[G.depart][receveur]);
  p = viderPile (p);
  free (p);
}

void Floyd_Warshall (graphe G)
{
  int i, j, k;
  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < G.nb_sommets; j++)
	{
	  G.W[i][j] = INFINI;
	  G.P[i][j] = 0;
	  if (G.matrice_adj[i][j] == 1)
	    {
	      G.P[i][j] = i;
	    }
	}
    }
  for (i = 0; i < G.nb_sommets; i++)
    {
      G.P[i][i] = i;
      G.W[i][i] = 0;
      for (j = 0; j < G.nb_sommets; j++)
	{
	  if (G.matrice_adj[i][j] == 1)
	    {
	      G.W[i][j] = G.temps[i][j];
	    }
	}
    }
  for (k = 0; k < G.nb_sommets; k++)
    {
      for (i = 0; i < G.nb_sommets; i++)
	{
	  for (j = 0; j < G.nb_sommets; j++)
	    {
	      if (G.W[i][k] + G.W[k][j] < G.W[i][j])
		{
		  G.W[i][j] = G.W[i][k] + G.W[k][j];
		  G.P[i][j] = G.P[k][j];
		}
	    }
	}
    }
  for (i = 0; i < G.nb_sommets; i++)
    {
      if (G.W[i][i] < 0)
	{
	  printf ("Erreur\n");
	}
    }
  printf ("MATRICE DES TEMPS:\n");
  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < G.nb_sommets; j++)
	{
	  printf ("%3d ", G.W[i][j]);
	}
      printf ("\n");
    }
  printf ("\n");
  printf ("MATRICE DES PREDECESSEURS:\n");
  for (i = 0; i < G.nb_sommets; i++)
    {
      for (j = 0; j < G.nb_sommets; j++)
	{
	  printf ("%3d ", G.P[i][j]);
	}
      printf ("\n");
    }
}

int main (int argc, char *argv[])
{
  srand (time (NULL));
  graphe G1;
  G1 = alea_graphe (G1);
  ecrire_fichier_dot (G1, argv[1]);
  auxiliaire (G1);
  G1 = parcours_profondeur (G1);
  Floyd_Warshall (G1);
  chemin_entre_deux (G1);
  libere_graphe (G1);

  //~ graphe G2;
  //~ G2 = alea_graphe_test(10,G2);
  //~ ecrire_fichier_dot2(G2,"1"); 
  //~ auxiliaire(G2);
  //~ Floyd_Warshall(G2);
  //~ chemin_entre_deux(G2);
  //~ libere_graphe(G2);
  exit (0);
}
