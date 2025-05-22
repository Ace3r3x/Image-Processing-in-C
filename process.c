/* This coursework specification, and the example code provided during the
 * course, is Copyright 2025 Heriot-Watt University.
 * Distributing this coursework specification or your solution to it outside
 * the university is academic misconduct and a violation of copyright law. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. */
struct Pixel {
    int red;
    int green;
    int blue;
};

/* An image loaded from a file. */
struct Image {
    /* TODO: Question 2 */
    int height;
    int width;
    struct Pixel *pixels;
};

/* Free a struct Image */
void free_image(struct Image *img)
{
    /* TODO: Question 3a */
    if (img) {
        free(img->pixels);
        free(img);
    }
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Allocate the Image object, and read the image from the file */
    /* TODO: Question 3b */
    struct Image *img = malloc(sizeof(struct Image));
    if (img == NULL) {
        fclose(f);
        return NULL;
    }

    // Read HPDEC header
    char format[6];
    if (fscanf(f, "%5s", format) != 1 || strcmp(format, "HPDEC") != 0) {
        fprintf(stderr, "Invalid file format\n");
        free(img);
        fclose(f);
        return NULL;
    }

    // Read dimensions
    if (fscanf(f, "%d %d", &img->height, &img->width) != 2 || 
        img->height <= 0 || img->width <= 0) {
        fprintf(stderr, "Invalid dimensions\n");
        free(img);
        fclose(f);
        return NULL;
    }

    // Allocate pixel data array
    img->pixels = malloc(img->height * img->width * sizeof(struct Pixel));
    if (img->pixels == NULL) {
        free(img);
        fclose(f);
        return NULL;
    }

    // Read pixel data
    for (int i = 0; i < img->height * img->width; i++) {
        if (fscanf(f, "%d %d %d", 
                  &img->pixels[i].red, 
                  &img->pixels[i].green, 
                  &img->pixels[i].blue) != 3 ||
            img->pixels[i].red < 0 || img->pixels[i].red > 255 ||
            img->pixels[i].green < 0 || img->pixels[i].green > 255 ||
            img->pixels[i].blue < 0 || img->pixels[i].blue > 255) {
            fprintf(stderr, "Invalid pixel data\n");
            free(img->pixels);
            free(img);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return img;
}

/* Write img to file filename. Return true on success, false on error. */
bool save_image(const struct Image *img, const char *filename)
{
    /* TODO: Question 3c */
    if (img == NULL || img->pixels == NULL || filename == NULL) {
        return false;
    }

    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        return false;
    }

    // Write header
    if (fprintf(f, "HPDEC\n%d %d\n", img->height, img->width) < 0) {
        fclose(f);
        return false;
    }

    // Write pixel data
    for (int i = 0; i < img->height * img->width; i++) {
        if (fprintf(f, "%d %d %d\n", 
                   img->pixels[i].red,
                   img->pixels[i].green,
                   img->pixels[i].blue) < 0) {
            fclose(f);
            return false;
        }
    }

    fclose(f);
    return true;
}

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{
    /* TODO: Question 3d */
    if (source == NULL || source->pixels == NULL) {
        return NULL;
    }

    struct Image *copy = malloc(sizeof(struct Image));
    if (copy == NULL) {
        return NULL;
    }

    copy->height = source->height;
    copy->width = source->width;

    size_t data_size = source->height * source->width * sizeof(struct Pixel);
    copy->pixels = malloc(data_size);
    if (copy->pixels == NULL) {
        free(copy);
        return NULL;
    }

    memcpy(copy->pixels, source->pixels, data_size);
    return copy;
}

/* Perform your first task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns a new struct Image containing the result, or NULL on error. */
struct Image *apply_NOISE(const struct Image *source, int strength)
{
    if (source == NULL || strength < 0) {
        return NULL;
    }

    struct Image *result = copy_image(source);
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < result->height * result->width; i++) {
        int noise_r = (rand() % (2 * strength + 1)) - strength;
        int noise_g = (rand() % (2 * strength + 1)) - strength;
        int noise_b = (rand() % (2 * strength + 1)) - strength;

        int new_r = result->pixels[i].red + noise_r;
        int new_g = result->pixels[i].green + noise_g;
        int new_b = result->pixels[i].blue + noise_b;

        result->pixels[i].red = (new_r < 0) ? 0 : (new_r > 255) ? 255 : new_r;
        result->pixels[i].green = (new_g < 0) ? 0 : (new_g > 255) ? 255 : new_g;
        result->pixels[i].blue = (new_b < 0) ? 0 : (new_b > 255) ? 255 : new_b;
    }

    return result;
}

/* Perform your second task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns true on success, or false on error.
* TODO: Question 5 */
bool apply_HIST(const struct Image *source)
{
    if (source == NULL || source->pixels == NULL) {
        return false;
    }

    unsigned long histogram[256] = {0};

    for (int i = 0; i < source->height * source->width; i++) {
        histogram[source->pixels[i].red]++;
        histogram[source->pixels[i].green]++;
        histogram[source->pixels[i].blue]++;
    }

    for (int i = 0; i < 256; i++) {
        printf("Value %d: %lu pixels\n", i, histogram[i]);
    }

    return true;
}

int main(int argc, char *argv[])
{
    /* Initialise the random number generator, using the time as the seed */
    /* TODO: Remove this if you don't need to use rand() */
    srand(time(NULL));

    /* Check command-line arguments */
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: process INPUTFILE OUTPUTFILE [NOISE_STRENGTH]\n");
        return 1;
    }

    int noise_strength = 5;  // default value
    if (argc == 4) {
        noise_strength = atoi(argv[3]);
    }

    /* Load the input image */
    struct Image *in_img = load_image(argv[1]);
    if (in_img == NULL) {
        return 1;
    }

    /* Apply the first process */
    struct Image *out_img = apply_NOISE(in_img, noise_strength);
    if (out_img == NULL) {
        fprintf(stderr, "Noise application failed.\n");
        free_image(in_img);
        return 1;
    }

    /* Apply the second process */
    if (!apply_HIST(out_img)) {
        fprintf(stderr, "Histogram generation failed.\n");
        free_image(in_img);
        free_image(out_img);
        return 1;
    }

    /* Save the output image */
    if (!save_image(out_img, argv[2])) {
        fprintf(stderr, "Saving image to %s failed.\n", argv[2]);
        free_image(in_img);
        free_image(out_img);
        return 1;
    }

    free_image(in_img);
    free_image(out_img);
    return 0;
}