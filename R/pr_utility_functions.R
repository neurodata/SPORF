# utility function for mnist data
library(reshape)
min.n <- function(x,n,value=TRUE){ 
  s <- sort(x, index.return=TRUE) 
  if(value==TRUE){s$x[n]} else{s$ix[n]}} 


# i is the number of row in the distance matrix D
calculate_precision_recall_for_each_row<-function(k, D, i, truth){
  # the intersection between relevant and retrevial 
  rele_ret = 0
  d=D[i, ]
  indices=seq(1, length(d))
  #create a dataframe
  df = data.frame(indices, d)
  colnames(df) <- c("ind", "dist")
  #sort dataframe based on distance
  df_sorted <- df[order(df$dist),]
  #select the indices corresponding to the least k distances
  k_nn_index_list <- subset(df_sorted, ind!=i)[1:k,1]
  for (index in k_nn_index_list){
    if(truth[index] == truth[i]){
      rele_ret = rele_ret+1
    }
  }
  precision = rele_ret/(k)
 
  # how many images in the ith row has the same label as the ith pic
  # this is equivalent to see 
  # how many images in the WHOLE DATSET has the same label as the current row (pic)
  recall = rele_ret/sum(truth==truth[i])
  return(list("prec"=precision, "rec"=recall))
}

########################################################

p_r_list<-function(D, data_label, at_K, num_of_points){
  precision_list=c()
  recall_list=c()
  # k is the number of nn to tale
  for (k in at_K){
    # fix the number of nns
    # i is the number of rows
    i=1
    # list that store p and c for each row of the D matrix
    precision_at_k_list=c()
    recall_at_k_list=c()
    while(i <= num_of_points){
      # for each row, calculate the p
      precision_and_recall = calculate_precision_recall_for_each_row(k, D, i, data_label)
      precision_at_k_ith_row=precision_and_recall$prec
      recall_at_k_ith_row=precision_and_recall$rec
      # store them in a list
      precision_at_k_list=append(precision_at_k_list, precision_at_k_ith_row)
      recall_at_k_list=append(recall_at_k_list, recall_at_k_ith_row)
      # go to the next row
      i=i+1
    }
    # average of the list is the p and c at each k
    precision_at_k=mean(precision_at_k_list)
    recall_at_k=mean(recall_at_k_list)
    # store the average p and c for each k into a list
    precision_list=append(precision_list, precision_at_k)
    recall_list=append(recall_list, recall_at_k)
  }
  return(list("precisionList"=precision_list, "recallList"=recall_list))
}

