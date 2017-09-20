import sys
import time
sys.setrecursionlimit(30000)
from adjacencygraph import UndirectedAdjacencyGraph
import random

def make_graph():
    '''
    Function used for creating of graph object. The sudoku board graph object
        is used for the duration of the game, the only changes occur are in the
        colours of certain verticies.

    Complexity:
    As the sudoku boards will always be 9x9, the run time is constant. However,
        if one were to attempt to generalize the process, the big O runtime
        at worst would be O(|V|^2*|BRC|), where V are vertices
        and BRC is the product of number of rows columns and boxes, which
        where |V| is the number of vertices in the graph being constructed
        |B| is number of boxes, rows and columns. Despite this being a heavy
        souding runtime, this function only needs to be called once per game,
        saving time.

    Returns:
        The sudoku grpah object.
    '''

    vertices = [x for x in range(1,82)]

    # O(n)
    row1 = set(vertices[0:9])
    row2 = set(vertices[9:18])
    row3 = set(vertices[18:27])
    row4 = set(vertices[27:36])
    row5 = set(vertices[36:45])
    row6 = set(vertices[45:54])
    row7 = set(vertices[54:63])
    row8 = set(vertices[63:72])
    row9 = set(vertices[72:81])

    rows = [row1,row2,row3,row4,row5,row6,row7,row8,row9]

    # O(n)
    col1 = {1+9*n for n in range(0,9)}
    col2 = {2+9*n for n in range(0,9)}
    col3 = {3+9*n for n in range(0,9)}
    col4 = {4+9*n for n in range(0,9)}
    col5 = {5+9*n for n in range(0,9)}
    col6 = {6+9*n for n in range(0,9)}
    col7 = {7+9*n for n in range(0,9)}
    col8 = {8+9*n for n in range(0,9)}
    col9 = {9+9*n for n in range(0,9)}

    columns=[col1,col2,col3,col4,col5,col6,col7,col8,col9]

    square1 = set([1,2,3,10,11,12,19,20,21])
    square2 = set([4,5,6,13,14,15,22,23,24])
    square3 = set([7,8,9,16,17,18,25,26,27])
    square4 = set([28,29,30,37,38,39,46,47,48])
    square5 = set([31,32,33,40,41,42,49,50,51])
    square6 = set([34,35,36,43,44,45,52,53,54])
    square7 = set([55,56,57,64,65,66,73,74,75])
    square8 = set([58,59,60,67,68,69,76,77,78])
    square9 = set([61,62,63,70,71,72,79,80,81])

    squares=[square1, square2, square3, square4, square5, square6, square7, square8, square9]

    graph = UndirectedAdjacencyGraph()
    for i in range(1,82):
        graph.add_vertex(i)

    for v in graph.vertices():  # O(|V|)
        for w in graph.vertices(): # O(|N|)
            for square in squares:  # O(9)
                if v in square and w in square and v!=w: # O(1)
                    if not graph.is_edge((v,w)):
                        graph.add_edge((v,w))   # O(1)
            for row in rows:  # O(9)
                if v in row and w in row and v!=w: # O(1)
                    if not graph.is_edge((v,w)):
                        graph.add_edge((v,w))
            for column in columns:  # O(9)
                if v in column and w in column and v!=w: # O(1)
                    if not graph.is_edge((v,w)):
                        graph.add_edge((v,w))

    return graph



def add_colours(graph, colour_list):
    '''Utility function for find_peak that uses a binary search to find the
    'peak' of the hill array.

    Parameters:
        graph (UndirectedAdjacencyGraph): the current graph object being updated
        with the colour list colour_list: a list of ints in range 1-9 representing
        the colours of the verticies in the graph. This is to be added to graph

    Complexity: O(|V|)
        Where V is the verticies in the graph (always 81)

    '''
    for i in range(1,82):
        graph.add_colour(i, colour_list[i-1])

def solve_check(graph):
    """
    Solve check routine for the sudoku boards. checks if there is a conflicting
        assignment of colours amongst neighbours.

    Parameters:
        graph(UndirectedAdjacencyGraph): the current graph object being checked
        for validitiy

    Complexity: O(|V|*|N|)
        Where |V| are the number of vertices and |N| is the number of
        neighbours.

    Returns:
        1 if correct board
        0 if incorrect
    """
    for v in graph.vertices():
        for w in graph.neighbours(v):
            if graph.colour(w)==graph.colour(v):
                return 0
    return 1


def find_empty_spot(graph):
    """
    Used to find a vertex in the graph with no colour assignment. Used in the
        solving funciton.

    Parameters:
        graph(UndirectedAdjacencyGraph): the current graph object

    Complexity: O(|V|)
        Where |V| are the number of vertices
        neighbours.

    Returns:
        (True,v) if there is an empty vertex, where v is the vertex
        (False, False) if no empty spots available
    """
    for v in graph.vertices():
        if graph.colour(v)==0:
            return (True,v)
    return (False,False)

def check_posibility(graph, v, colour):
    """
    Checks if a current assignment is valid

    Parameters:
        graph: the current graph object
        v: the vertex being checked
        colour: the colour being checked for validity at vertex v

    Complexity: O(|N|)
        Where |N| are the number of neighbours.
    """
    for w in graph.neighbours(v):
        if graph.colour(w)==colour:
            return False
    return True

def solve(graph):
    """
    Solves a sudoku board represented by a graph by using a recursive backtracking
        approach.

    Parameters:
        graph: the current graph object to be solved

    Returns:
        True if graph is solved correctly
        False if graph cannot be solved
    """
    global t
    solved = find_empty_spot(graph) # O(|V|)
    if not solved[0]:
        return solve_check(graph)
    else:
        v = solved[1]
        for colour in range(1,10): # O(10)
            if (time.time() - t) > 5:  # if takes longer than 5 seconds to solve, bad board
                return False
            if check_posibility(graph, v, colour): #O(|N|)
                graph.add_colour(v, colour) # O(1)

                if solve(graph): # O(|V|) calls
                    return True

                graph.add_colour(v, 0) # O(1)
        return False  # backtracking if needed

def generate_solved_board():
    """
    Generates a random solved board to be used in conjuction with generate_board
        to create a random sudoku board for the game.

    Returns:
        The randomly solved graph to be used in generate_board
    """
    graph = make_graph()
    v_list=random.sample(graph.vertices(), 9)
    counter = 1
    for v in v_list:  # O(9)
        graph.add_colour(v, counter)
        counter+=1
    if solve(graph):
        return graph
    else:
        return generate_solved_board()

def generate_board(difficulty):
    """
    Generates a random sudoku board for the game.

    Returns:
        The colour list of the generated board.
    """
    graph = generate_solved_board()
    v_list = random.sample(graph.vertices(), 81-difficulty)
    for v in v_list:
        graph.add_colour(v, 0)
    return graph.colours()


if __name__ == "__main__":
    # Tests
    colour_list=[1,5,2,4,6,9,3,7,8,7,8,9,2,1,3,4,5,6,4,3,6,5,8,7,2,9,1,6,1,
    3,8,7,2,5,4,9,9,7,4,1,5,6,8,2,3,8,2,5,9,3,4,1,6,7,5,6,7,3,4,8,9,1,2,2,4,
    8,6,9,1,7,3,5,3,9,1,7,2,5,6,8,4]

    colour_list_2 = [9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

    c = [0,1,0,0,2,3,6,0,7,0,2,8,4,6,0,0,0,0,0,0,0,0,0,8,4,0,5,8,9
    ,0,0,0,0,7,4,0,0,0,2,0,0,0,5,0,0,0,5,6,0,0,0,0,1,9,9,0,5,3,0,0,0,0,0,0,0,0,0,
    4,5,9,6,0,3,0,4,6,1,0,0,5,0]
    graph = make_graph()
    n=9
    """for i in range(1,82):
        if i in range(1,10):
            graph.add_colour(i,n)
            n-=1
        else:
            graph.add_colour(i,0)"""

    """t = time.time()
    #board = generate_board1(graph, 20)
    #print([graph.colour(v) for v in graph.vertices()])
    #print("borad", board)
    add_colours(graph, board)
    t = time.time()
    print("did it solve?:", solve(graph))
    print(solve_check(graph))"""


    """board = [1, 2, 3, 4 ,5, 6, 7, 8, 9,
            10,11,12,13,14,15,16,17,18,
            19,20,21,22,23,24,25,26,27,
            28,29,30,31,32,33,34,35,36,
            37,38,39,40,41,42,43,44,45,
            46,47,48,49,50,51,52,53,54,
            55,56,57,58,59,60,61,62,63,
            64,65,66,67,68,69,70,71,72,
            73,74,75,76,77,78,79,80,81]
    puzzle = generate_board(graph)
    print([puzzle.colour(v)for v in puzzle.vertices()])

    board=generate_board(graph)
    print("Board: ", board)
    counter = 0
    for b in board:
        if b!=0:
            counter+=1
    print(counter)
"""
