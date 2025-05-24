#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DIGITS 128 // 1024 bits / 8 bits per digit = 128 digits

typedef struct {
    int digits[MAX_DIGITS];  // Store digits as integers (0-9)
    int length;              // Actual length of the number (ignores leading zeros)
    int isNegative;          // Flag to indicate if the number is negative (0 = false, 1 = true)
} BigInt;
void BigInt_Init(BigInt *num, const char *str) 
{
    int len = strlen(str);
    num->isNegative = (str[0] == '-') ? 1 : 0; // Check if negative (0 = false, 1 = true)
    int startIndex = num->isNegative ? 1 : 0;   // Skip '-' if negative

    num->length = 0; // Reset length

    // Process the string in reverse order to store digits
    for (int i = len - 1; i >= startIndex; --i) 
    {
        num->digits[num->length++] = str[i] - '0'; // Store digits in reverse order
    }
}

// Function to print a BigInt
void BigInt_Print(const BigInt *num) 
{
    if (num->isNegative) 
    {
        printf("-");
    }

    if (num->length == 0) {
        printf("0");
    } else 
    {
        for (int i = num->length - 1; i >= 0; --i) 
        {
            printf("%d", num->digits[i]);
        }
    }
    printf("\n");
}

// Function to add two BigInts
void BigInt_Add(BigInt *result, const BigInt *a, const BigInt *b) 
{
    int carry = 0;
    int i = 0;
    result->length = 0;

    // Both numbers have the same sign
    if (a->isNegative == b->isNegative) 
    {
        // Add the digits
        while (i < a->length || i < b->length || carry != 0) 
        {
            int sum = carry;

            if (i < a->length) 
            {
                sum += a->digits[i];
            }
            if (i < b->length) 
            {
                sum += b->digits[i];
            }

            result->digits[result->length++] = sum % 10;
            carry = sum / 10;
            i++;
        }

        // The result has the same sign as the input numbers
        result->isNegative = a->isNegative;
    }
    // Case 2: One number is positive and the other is negative
    else 
    {
        // Determine which number has the larger magnitude
        BigInt *larger = NULL;
        BigInt *smaller = NULL;

        // Compare the lengths (magnitude of the number)
        if (a->length > b->length) {
            larger = (BigInt *)a;
            smaller = (BigInt *)b;
        } else if (a->length < b->length) {
            larger = (BigInt *)b;
            smaller = (BigInt *)a;
        } else {
            // If lengths are equal, compare digit by digit from the most significant digit
            int j = a->length - 1;
            while (j >= 0 && a->digits[j] == b->digits[j]) {
                j--;
            }
            if (j < 0 || a->digits[j] > b->digits[j]) {
                larger = (BigInt *)a;
                smaller = (BigInt *)b;
            } else {
                larger = (BigInt *)b;
                smaller = (BigInt *)a;
            }
        }

        // Now perform subtraction (a - b or b - a depending on the larger)
        int borrow = 0;
        i = 0;
        result->length = 0;

        while (i < larger->length || borrow != 0) {
            int diff = larger->digits[i] - borrow;

            if (i < smaller->length) {
                diff -= smaller->digits[i];
            }

            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }

            result->digits[result->length++] = diff;
            i++;
        }

        // Remove trailing zeros
        while (result->length > 1 && result->digits[result->length - 1] == 0) {
            result->length--;
        }

        // The result takes the sign of the larger number
        result->isNegative = larger->isNegative;
    }
}

// Copy the contents of one BigInt to another

void BigInt_CopyAbsolute(BigInt *dest, const BigInt *src) 
{
    
    dest->length = src->length;
    for (int i = 0; i < src->length; i++) 
    {
        dest->digits[i] = src->digits[i];
    }
    dest->isNegative = 0;
}

// Compare two BigInt numbers
int BigInt_Compare(const BigInt *a, const BigInt *b) 
{
    // Compare the lengths first
    if(a->length > b->length) 
    {
    return 1; 
    } // a is greater
    else if(a->length < b->length)
    { 
    return -1;
    }
    else if(a->length==b->length)
    {
    // If lengths are equal
    for (int i = a->length - 1; i >= 0; i--) 
    {
        if (a->digits[i] > b->digits[i]) 
        return 1;  // a is greater
        if (a->digits[i] < b->digits[i]) 
        return -1; 
        else 
        return 0;// b is greater
    }
    }// Both numbers are equal
}
void BigInt_Subtract(BigInt *result, const BigInt *a, const BigInt *b) 
{
    int borrow = 0;
    int i = 0;
    result->length = 0;

    // Case 1: Both numbers are positive
    if (a->isNegative==0 && b->isNegative==0) {
        int comparison = BigInt_Compare(a, b);

        if (comparison == 0) {
            // If both numbers are equal
            result->length = 1;
            result->digits[0] = 0;
            result->isNegative = 0; // Zero is positive
            return;
        } else if (comparison > 0) {
            // a is larger than b (a - b)
            while (i < a->length || borrow != 0) {
                int diff = a->digits[i] - borrow;
                if (i < b->length) {
                    diff -= b->digits[i];
                }

                if (diff < 0) {
                    diff += 10;
                    borrow = 1;
                } else {
                    borrow = 0;
                }

                result->digits[result->length++] = diff;
                i++;
            }

            // Remove trailing zeros
            while (result->length > 1 && result->digits[result->length - 1] == 0) 
            {
                result->length--;
            }

            result->isNegative = 1; // Result is positive
        } else {
            // b is larger than a (b - a)
            while (i < b->length || borrow != 0) {
                int diff = b->digits[i] - borrow;
                if (i < a->length) {
                    diff -= a->digits[i];
                }

                if (diff < 0) {
                    diff += 10;
                    borrow = 1;
                } else {
                    borrow = 0;
                }

                result->digits[result->length++] = diff;
                i++;
            }

            // Remove trailing zeros
            while (result->length > 1 && result->digits[result->length - 1] == 0) {
                result->length--;
            }

            result->isNegative = 1; // Result is negative
        }
    }
    // Case 2: One number is positive and the other is negative (a + |b| or |a| + b)
    else if (a->isNegative && !b->isNegative) {
        // a is negative, b is positive: effectively -a - (b) = a + b
        BigInt aAbs, bAbs;
        BigInt_Init(&aAbs, "");
        BigInt_Init(&bAbs, "");
        BigInt_CopyAbsolute(&aAbs,a);  // Copy absolute value of a
        BigInt_CopyAbsolute(&bAbs,b);  // Initialize bAbs
        BigInt_CopyAbsolute(&bAbs,b);  // Copy the absolute value of b
        BigInt_Add(result,&aAbs,&bAbs);   // Add a and abs(b)
        result->isNegative = 1;        // Result will be positive since we treat it as addition
    } else if (!a->isNegative && b->isNegative) 
    {
        // a is positive, b is negative: effectively a + |b|
        BigInt bAbs;
        BigInt_Init(&bAbs, "");// Copy absolute value of a
        BigInt_CopyAbsolute(&bAbs,b);  // Initialize bAbs
        BigInt_Init(&bAbs, "");       // Initialize aAbs
        BigInt_CopyAbsolute(&bAbs,b);  // Copy the absolute value of a
        BigInt_Add(result,&bAbs, a);   // Add abs(a) and b
        result->isNegative = 0;        // Result will be positive
    }
    // Case 3: Both numbers are negative (-a-(-b)=-a+b)
    else if (a->isNegative && b->isNegative) {
        BigInt bAbs,aAbs;
        BigInt_Init(&aAbs, "");
        BigInt_Init(&bAbs, "");
        BigInt_CopyAbsolute(&aAbs,a);  // Copy absolute value of a
        BigInt_CopyAbsolute(&bAbs,b);  // Initialize bAbs

        int comparison = BigInt_Compare(&aAbs,&bAbs);
        
        if (comparison == 0) {
            // Both absolute values are equal
            result->length = 1;
            result->digits[0] = 0;
            result->isNegative = 0; // Zero is positive
        } else if (comparison > 0) {
            // |a| is larger than |b|, result will be negative: |a| - |b|
            BigInt_Subtract(result,&aAbs,&bAbs); // |a| - |b|
            result->isNegative = 1; // Result is negative
        } else {
            // |b| is larger than |a|, result will be negative: |b| - |a|
            BigInt_Subtract(result, &bAbs,&aAbs); // |b| - |a|
            result->isNegative = 0; // Result is negative
        }
    }
}


// Function to multiply two BigInts
void BigInt_Multiply(BigInt *result, const BigInt *a, const BigInt *b) {
    // Initialize result as zero
    for (int i = 0; i < MAX_DIGITS; i++) {
        result->digits[i] = 0;
    }
    result->length = 0;

    // Multiply using long multiplication
    for (int i = 0; i < a->length; i++) {
        int carry = 0;
        for (int j = 0; j < b->length; j++) {
            int product = a->digits[i] * b->digits[j] + result->digits[i + j] + carry;
            result->digits[i + j] = product % 10;
            carry = product / 10;
        }

        if (carry != 0) {
            result->digits[i + b->length] += carry;
        }
    }

    // Determine the actual length of the result
    result->length = a->length + b->length;
    while (result->length > 1 && result->digits[result->length - 1] == 0) {
        result->length--;
    }
    if((a->isNegative && !b->isNegative)|| (b->isNegative && !a->isNegative))
    {
        result->isNegative=1;
    }
    else{
        result->isNegative=0;
    }
}

int main() {
    BigInt a, b, result;
    char inputA[MAX_DIGITS + 1], inputB[MAX_DIGITS + 1];

    // Prompt user for input
    printf("Enter the first BigInt number (up to 128 digits): ");
    scanf("%128s", inputA);  // Reads a string of up to 128 characters

    printf("Enter the second BigInt number (up to 128 digits): ");
    scanf("%128s", inputB);  // Reads a string of up to 128 characters

   // printf("Enter the first BigInt number (up to 128 digits): ");
    //fgets(inputA, sizeof(inputA), stdin);
    //inputA[strcspn(inputA, "\n")] = 0; // Remove trailing newline character

    //printf("Enter the second BigInt number (up to 128 digits): ");
    //fgets(inputB, sizeof(inputB), stdin);
    //inputB[strcspn(inputB, "\n")] = 0; // Remove trailing newline character

    // Initialize BigInt numbers from user input
    BigInt_Init(&a, inputA);
    BigInt_Init(&b, inputB);

    // Addition
    BigInt_Add(&result, &a, &b);
    printf("Sum: ");
    BigInt_Print(&result);

    // Subtraction
    BigInt_Subtract(&result, &a, &b);
    printf("Difference: ");
    BigInt_Print(&result);

    // Multiplication
    BigInt_Multiply(&result, &a, &b);
    printf("Product: ");
    BigInt_Print(&result);

    return 0;
}
