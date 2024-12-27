#!/bin/bash
gcc cliente.c -o cliente
while true; do
	resposta=$(dialog --stdout --title "Sistema de Mensagem Simples" --menu "Escolha uma opção:" \
	                0 0 20 \
	                1 "Enviar mensagem" \
	                2 "Ver mensagens recebidas" \
	                3 "Sair")
	
	case $resposta in
		1)
		destinatario=$(dialog --stdout --inputbox "Para quem deseja enviar?" 0 20)
		msg=$(dialog --stdout --inputbox "Escreva a mensagem:" 0 20)
		./cliente 0 $destinatario "$msg";;
		2)
		./cliente 1 0
		recebidas=$(cat recebidas.txt)
		echo $recebidas
		dialog --stdout --msgbox "$recebidas" 20 100;;
		3)
		rm recebidas.txt
		clear
		break
	esac
done
