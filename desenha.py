import cv2 as cv 
import numpy as np
import pandas as pd
eq = 0
with open("Netlists.spice","r") as f, open("Nets.txt","r") as net_file:
    #abre spice e ignora primeiros 3 linha(sem netlist)
    for _ in range(1):
        next(f)
    linha = f.readline()
    while(len(linha) != 0):
        eq+=1
        #Baseado em como inicia a linha, pega a equacao, ou o netlist
        if linha.startswith("A"):
            _, _, _, equacao = linha.split()
            print(equacao)

        #Guarda netlist em um dataframe
        linha = f.readline()
   
        netlist = pd.DataFrame(columns=['Drain', 'Gate', 'Source','Bulk','Type'])
        while linha and linha.startswith("M"):
            _, drain, gate, source, bulk, tipo = linha.split()
            #print(numero,drain,gate,source,bulk,tipo)
            netlist.loc[len(netlist)] = [drain, gate, source, bulk, tipo]
            linha = f.readline()

        print(netlist)

    #Cria uma imagem vazia
        n = netlist.shape[0]//2+1           #Comprimento da celula + 1(número de terminais efetivos)
        blank = np.zeros((1000,n*100,3), dtype='uint8')
        cv.rectangle(blank,(0,0),(n*100,1000),(255,255,255), thickness=-1)

    #Abre txt e pega linhas de roteamento
        net = net_file.readline()
        if(net.startswith("!")):
            net = net_file.readline()
        while(net.startswith("n") or net.startswith("Z")):
            nome, inicio, fim, _, num_lin = net.split()
            
            inicio = int(inicio)-1                                              #pos começa em 1, mudando para começar em 0
            fim = int(fim)
            if(inicio % 2 != 0):
                inicio = inicio+1                                               #ignorar pos impares, compartilhamento de difusão
            if(fim % 2 == 0):
                fim = fim+1
            cv.line(blank,(inicio*blank.shape[1]//(n*2) + 50,(blank.shape[0]//3 + 50*int(num_lin))),(fim*blank.shape[1]//(n*2),(blank.shape[0]//3 + 50*int(num_lin))),(255,0,255),8)
            cv.putText(blank,nome,(inicio*blank.shape[1]//(n*2-2),(blank.shape[0]//3 + 50* int(num_lin))), cv.FONT_HERSHEY_PLAIN, 2.0,(255,0,0))
            net = net_file.readline()
            print(nome," ", inicio, " ", fim)
        x_p = 0
        x_n = 0
        #blank[:] = 0,255,0
        #cv.imshow('Green',blank)
        for index,row in netlist.iterrows():
            if(row['Type'] == "pfet"):
                cv.line(blank,(x_p * blank.shape[1]//n,blank.shape[0]//6), ((blank.shape[1]//n)*(x_p+1), blank.shape[0]//6 ), (0,255,0),15) #desenha difusao pull up
                cv.putText(blank,row['Drain'],(x_p * blank.shape[1]//n + 10,blank.shape[0]//6 - 10), cv.FONT_HERSHEY_PLAIN, 1.0,(255,0,0))
                x_p+=1
                if(x_p == n - 1):
                    cv.line(blank,(x_p * blank.shape[1]//n,blank.shape[0]//6), ((blank.shape[1]//n)*(x_p+1), blank.shape[0]//6), (0,255,0), 15) #desenha difusao pull up
                cv.putText(blank,row['Source'],(x_p * blank.shape[1]//n + 10,blank.shape[0]//6 - 10), cv.FONT_HERSHEY_PLAIN, 1.0,(255,0,0))

            if(row['Type'] == "nfet"):
                cv.line(blank,(x_n * blank.shape[1]//n,(blank.shape[0]//3)*2 +(blank.shape[0]//5)), ((blank.shape[1]//n)*(x_n+1), (blank.shape[0]//3)*2 +(blank.shape[0]//5)), (0,255,0), 15) # desenha difucao pull down
                cv.putText(blank,row['Drain'],(x_n * blank.shape[1]//n + 10,(blank.shape[0]//3)*2 +(blank.shape[0]//5) - 10), cv.FONT_HERSHEY_PLAIN, 1.0, (255,0,0))
                x_n+=1
                if(x_n == n - 1):
                    cv.line(blank,(x_n * blank.shape[1]//n,(blank.shape[0]//3)*2 +(blank.shape[0]//5)), ((blank.shape[1]//n)*(x_n+1), (blank.shape[0]//3)*2 +(blank.shape[0]//5)), (0,255,0), 15) # desenha difucao pull down
                cv.putText(blank,row['Source'],(x_n * blank.shape[1]//n + 10,(blank.shape[0]//3)*2 +(blank.shape[0]//5) - 10), cv.FONT_HERSHEY_PLAIN, 1.0, (255,0,0))
        x_p = 0
        #desenha polis
        for index,row in netlist.iterrows():
            if(row['Type'] == "pfet"):
                if(row['Gate'] != "Z"):
                    cv.line(blank, ((blank.shape[1]//n)*(x_p+1),0),((blank.shape[1]//n)*(x_p+1),blank.shape[0]), (0,0,255),15)
                    cv.putText(blank,row['Gate'],((blank.shape[1]//n)*(x_p+1)-10,blank.shape[0]//2),cv.FONT_HERSHEY_PLAIN, 2.0, (255,0,0))
                else:
                    cv.line(blank, ((blank.shape[1]//n)*(x_p+1),0),((blank.shape[1]//n)*(x_p+1),blank.shape[0]), (255,255,255),30)
                x_p+=1

        filename = "./img/"+equacao+".jpg"
        print(filename)
        cv.imwrite(filename,blank)
#cv.imshow('Blank',blank)
#cv.waitKey(0)