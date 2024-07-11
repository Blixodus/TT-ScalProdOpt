from Scripts.contraction_list import generate_contraction_list
from ast import literal_eval

# Read the recursive definition of contraction order
order_str = input("Enter the recursive contraction order: ")

# Parse string into tuple object
order = literal_eval(order_str)

# Generate the flat list of contractions
contraction_list, _ = generate_contraction_list(order)
print(contraction_list)