
org 100h
jmp main						; jump to main.
message: db 'Hello world!', 0
print:              
	mov ah, 0eh					; int 10h 'print char' function	
	._loop:
	lodsb						; Load byte at DS:[SI] into AL. Update SI.
	cmp al, 0					; compare al with 0 value.
	je .done					; jump to done if al equal 0.
		
	int 10h						; outputing character to screen.
	jmp ._loop					; loop for next characters.
.done:
	ret						; out of print function.
			
main:   
	mov si, message					; copy address of message to si register.
	call print
	ret
				
