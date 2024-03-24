
GeraDados <- function(diretorio) {
  arq_gen <- read.table(diretorio)
  vet_gen <- rep(arq_gen$V1);
  
  desloca <- 1;
  cat("\tMin. 1st Qu.  Median    Mean 3rd Qu.    Max.\n")
    
  
  
  otimo <- c(1698   ,
             1360   ,
             1158   ,
             3235   ,
             2623   ,
             2339   ,
             1931  ,
             1402  ,
             1243  ,
             3456  ,
             2806  ,
             2391  ,
             12673,
             11568,
             8431 ,
             24927,
             23302,
             22377)
  
  # 3 * 6 arquivos (a, c, d)
  for (j in 1:18) {
    cat(j, "\t");
    vetor_buffer <- vet_gen[desloca:(desloca + 9)];
    cat(summary(vetor_buffer), "\t\t")
    
    cat(sd(vetor_buffer))
    cat("\n")
    
    desloca = desloca + 10;
  }
  desloca = 1;
  for (j in 1:18) {
    #cat(j, "\t");
    vetor_buffer <- vet_gen[desloca:(desloca + 9)];
    cat(100 * (otimo[j] / min(vetor_buffer)))
    cat("\n")
    
    desloca = desloca + 10;
  }
  
}

GeraDados("out_genetico.txt")
GeraDados("out_simulated_annealing.txt")
GeraDados("out_reinicio.txt")
GeraDados("out_grasp.txt")
