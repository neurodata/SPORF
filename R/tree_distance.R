library(rerf)
library(datastructures)
# source('swiss_roll.R')

# type = "old"
type = "power"
# type = ""
# Generate data
# X <- SwissRoll(1000, Height=100, Plot = TRUE)[[1]]
X <- as.matrix(iris[, 1:4])

# Set some parameters
n_trees <- 50
min_parent <- 1

# Create the unsupervised Randomer Forest
Forest <- Urerf(X, trees = n_trees, min.parent = min_parent)

# Store the number of data 
n <- nrow(X)

# Create an n x n dissimilarity matrix



tree_distance <- function(Forest, type = 'power') {
  dissimilarity <- matrix(0, n, n)
  n_trees <- length(Forest$forest)
  n <- sqrt(length(Forest$similarityMatrix))
  for (tree in 1:n_trees) { # For each tree..
  
  # Initialize a stack to help traverse node paths
    s <- stack() 
    
    # Insert the root node
    insert(s, 1)  
    
    # Store the appropriate tree
    temp_tree <- Forest$forest[[tree]] 
    
    # Store the children df to help traverse node paths
    children_df = temp_tree$Children 
    
    # Grab the first node
    current_node = pop(s) 
    
    # Initialize a stack of paths to store node paths
    path_stack = stack() 
    
    # Push a path with just the root node
    insert(path_stack, c(1))
    
    # Store the number of nodes in the tree to 'zero pad' the paths
    n_nodes = nrow(children_df)
    
    # Initialize a matrix of paths
    path_matrix <- matrix(0, n, n_nodes)
    
    while (!is.null(current_node)){ # While there are still nodes to visit
      if (children_df[current_node, 1] == 0) { # Check if the node is a leaf node
        
        # Grab the path from the root node to the current node
        path_to_here <- pop(path_stack)
        
        # Pad the end of the path with zeros 
        # This allows for tree distances to be calculated by counting the non-zero
        # entries of PATH1 - PATH2
        final_path <- c(path_to_here, rep(0, n_nodes - length(path_to_here)))
        
        # Store the data indices of this leaf node
        current_indices <- temp_tree$ALeaf[[current_node]]
        
        # For each of the data indices of this leaf node, update the corresponding
        # row of path_matrix
        for (index in current_indices) {
          path_matrix[index, ] <- final_path
        } # end for
        
        # Next!
        current_node <- pop(s)
      } # end if
      
      else {
        # Grab the path from the root node to the current node
        path_to_here <- pop(path_stack)
        
        # Grab the left and right children of the current node
        left_child <- children_df[current_node, 1]
        right_child <- children_df[current_node, 2]
        
        # Push the left child
        insert(s, left_child)
        
        # Push the path to the left child
        path_left <- c(path_to_here, left_child)
        insert(path_stack, path_left)
        
        # Push the right child
        insert(s, right_child)
        
        # Push the path to the right child
        path_right <- c(path_to_here, right_child)
        insert(path_stack, path_right)
        
        # Next!
        current_node <- pop(s)
      } # end else
    } # end while 
    
    temp_dissimilary_matrix <- matrix(0, n, n)
    for (index1 in 1:n) { # for each pair of data points
      for (index2 in index1 : n) {
        
        # Store path vectors
        temp1 <- path_matrix[index1, ] 
        temp2 <- path_matrix[index2, ] 
        
        num_zeros1 <- sum(temp1 == 0)
        num_zeros2 <- sum(temp2 == 0)
        
        # Find difference in paths
        vector_difference <- temp1 - temp2
        
        # print(vector_difference)
        
        count_nonzeros <- sum(vector_difference != 0)
        
        total_dist <- count_nonzeros + (count_nonzeros - abs(num_zeros1 - num_zeros2))
        
        if (type == "old") { 
          dist = sum(total_dist != 0)
        } else if (type == "power") {
          dist = 1 - (1 / 2^total_dist)
        } else {
          dist = total_dist
        }
        
        # Update the corresponding dissimilarity matrix entry by adding the number of non-zero
        # entries in vector_difference
        temp_dissimilary_matrix[index1, index2] <- dist
        temp_dissimilary_matrix[index2, index1] <- dist
      } # end for
    } # end for
    max_dist <- max(temp_dissimilary_matrix)
    # dissimilarity <- dissimilarity + (temp_dissimilary_matrix / max_dist)
  } # end for

  # Normalize by the number of trees
  dissimilarity <- dissimilarity / n_trees
  return(dissimilarity)
}

