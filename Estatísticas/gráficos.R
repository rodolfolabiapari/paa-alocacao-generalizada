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
GeraDados <- function(d_ga, d_sa, d_r, d_gr) {
  arq_ga <- read.table(d_ga)
  arq_sa <- read.table(d_sa)
  arq_r <- read.table(d_r )
  arq_gr <- read.table(d_gr)
  
  ga <- rep(arq_ga$V1);
  sa <- rep(arq_sa$V1);
  r  <- rep(arq_r$V1);
  gr <- rep(arq_gr$V1);
  
  desloca <- 1;
  
  
  for (j in 1:18) {
    #png(paste(d_ga, "_", j, ".png", sep=""), width = 840, height = 480, units = "px");
    png(paste(j, ".png", sep=""), width = 840, height = 480, units = "px");
    
    boxplot(ga[desloca:(desloca + 9)], 
            sa[desloca:(desloca + 9)], 
            r[desloca:(desloca + 9)], 
            gr[desloca:(desloca + 9)],
            ylim = c(otimo[j] - 1, 
                     max(max(ga[desloca:(desloca + 9)]), max(sa[desloca:(desloca + 9)]), max(r[desloca:(desloca + 9)]), max(gr[desloca:(desloca + 9)])) + 1),
            main = paste("Instância ", j, sep = ""),
            xlab = "Algoritmos",
            ylab = "Custo", 
            col = "orange",
            border = "brown",
            names = c("Genético", "Simulated Annealing", "Reinício", "GRASP"));
    abline(h = otimo[j], col = "blue");
    
    dev.off()
    
    desloca = desloca + 10;
  }
}

GeraDados("out_genetico.txt", "out_simulated_annealing.txt", "out_reinicio.txt", "out_grasp.txt")
