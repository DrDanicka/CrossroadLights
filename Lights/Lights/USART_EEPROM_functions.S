.equ UCSR0A, 0xC0
.equ UCSR0B, 0xC1
.equ UCSR0C, 0xC2
.equ UBRR0L, 0xC4
.equ UBRR0H, 0xC5
.equ UDR0, 0xC6
.equ EECR,  0x3F
.equ EEDR,  0x40
.equ EEARL, 0x41
.equ EEARH, 0x42
.equ SPMCSR, 0x57

.equ TXEN0, 3
.equ RXEN0, 4
.equ RXC0, 7
.equ USBS0, 3
.equ EEPE,  1
.equ EERE,  0
.equ EEMPE, 2
.equ SPMEN, 0
.equ UCSZ01, 2
.equ UCSZ00, 1
.equ UDRE0, 5


.global USART_init
.global USART_ReadConfig
.global EEPROM_ReadByte
.global EEPROM_WriteByte
.global puts


.equ CONFIG_START, 0x0300		; Place buffer at address 0x0300


USART_Receive:
    lds r17, UCSR0A
    sbrs r17, RXC0
    rjmp USART_Receive

    lds r16, UDR0				; Store received byte into r16
    ret


USART_Transmit:
    lds r17, UCSR0A				; Read UCSR0A to r17
    sbrs r17, UDRE0				; Skip if UDRE is set
    rjmp USART_Transmit

    ; Send data in r16
    sts UDR0, r16
    ret


USART_init:
	; Set baud rate to 115200
    ldi r16, 0x08
	ldi r17, 0x00

	; Store to UBRR
	sts UBRR0H, r17
	sts UBRR0L, r16

	ldi r16, (1<<UCSZ01) | (1<<UCSZ00)		; Set 8 data bits and 1 stop bit
    sts UCSR0C, r16

    ldi r16, (1 << RXEN0) | (1 << TXEN0)	; Enable receive and transmit
    sts UCSR0B, r16

    ret



USART_ReadConfig:
    ldi r18, 0					; Counter for 10 bytes
    ldi r30, lo8(CONFIG_START)  ; Load buffer base address into Z (low byte)
    ldi r31, hi8(CONFIG_START)  ; Load buffer base address into Z (high byte)

Read_Loop:
    call USART_Receive
    st Z+, r16					; Store in RAM buffer
    
    inc r18
    cpi r18, 10					; Stop after 10 bytes
    brlo Read_Loop				; Continue loop if less than 10 bytes

    ; Now we have 10 bytes in RAM buffer -> write to EEPROM

	ldi r18, 0                   ; Reset counter for EEPROM write
    ldi r30, lo8(CONFIG_START)   ; Load buffer base address into Z (low byte)
    ldi r31, hi8(CONFIG_START)   ; Load buffer base address into Z (high byte)

Write_Loop:
    ld r16, Z+
    mov r24, r18                 ; Set low EEPROM address
	ldi r25, 0x00				 ; Set up EEPROM address
    call EEPROM_WriteByteSet     ; Write byte to EEPROM

    inc r18                      ; Increment counter
    cpi r18, 10                  ; Stop after writing 10 bytes
    brlo Write_Loop              ; Continue if more bytes left

    ret

EEPROM_WriteByte:
	mov r16, r22				; Get data argument from function
	ldi r25, 0x00				; Set up EEPROM address (low is as argument in r24)

EEPROM_WriteByteSet:
    cli							; Disable interrupts (written in documentation)

EEPROM_Wait:
	; sbic EECR,EEPE in documentation but rewritten becasue of out of range
    lds r17, EECR
    sbrc r17, EEPE				; Wait if EEPE is set
    rjmp EEPROM_Wait

SPM_Wait:
	; Adviced in documentation
    lds r17, SPMCSR
    sbrc r17, SPMEN				; Wait if SPMEN is set
    rjmp SPM_Wait

    ; Set EEPROM address
    sts EEARL, r24
    sts EEARH, r25

    sts EEDR, r16				; Load data into EEPROM register

    ; Eneable EEPROM write
    lds r17, EECR
    ori r17, (1 << EEMPE)		; Set EEMPE bit

	mov r19, r17
	ori r19, (1 << EEPE)		; Set EEPE bit

	; Set EECR behind each other because it has to be done in 4 cycles as written in documentation
    sts EECR, r17
	sts EECR, r19

    sei							; Re-enable interrupts
    ret


EEPROM_ReadByte:
    ; Wait for EEPROM ready
    ; sbic EECR, EEPE -> this was in documentation, but invalid becasue operand is out of range
	lds r17, EECR
	sbrc r17, EEPE
    rjmp EEPROM_ReadByte

    ; Set address
    ldi r25, 0x00
	sts EEARL, r24
    sts EEARH, r25

    ; Start read
    ; sbi EECR, EERE -> this was in documentation, but invalid becasue operand is out of range
	lds r17, EECR
	ori r17, (1 << EERE)
	sts EECR, r17
	
    lds r24, EEDR			; Read data register to r24 -> return register
    ret


puts:
	; Move adress to Z
    mov r30, r24
    mov r31, r25


send_next_char:
    ld r16, Z+
    tst r16					; Check if end of the string
    breq end_puts

    call USART_Transmit		; Transmit character in r16
    rjmp send_next_char

end_puts:
    ret
