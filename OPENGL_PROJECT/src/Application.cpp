#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Glewinit requires the above context (look at documentation).
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    //cordinates of our traingle
    float positions[6] = {
        -0.5f, -0.5f, //x and y coordaintes
         0.0f,  0.5f, //float is 4 bytes
         0.5f, -0.5f, //3 Vertex
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); //binding is like selecting a 'buffer' layer in photoshop
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); //sizeof is in bytes
   
    glEnableVertexAttribArray(0); //enabling for the next line to work
    //now we describe the layout
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); //(index, size, type, normalized, stride, pointer)
    //index (only 1 attrib here) so 0, size 1-4 (2 because we have x and y), data type, normalized = do they need converting type or memory location? (no), 
    //stride = offset to next vertex, pointer = offset between attributes in bytes)
    

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3); //use if you have no index buffer (mode, starting index, no. indicies to be rendered)

        //glDrawElements(GL_TRIANGLES, 3, null) is the other option for drawing

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}