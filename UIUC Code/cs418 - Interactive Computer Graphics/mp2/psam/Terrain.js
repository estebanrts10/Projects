/**
 * @file Terrain.js - A simple 3D terrain model for WebGL
 * @author Ian Rudnick <itr2@illinois.edu>
 * @brief Starter code for CS 418 MP2 at the University of Illinois at
 * Urbana-Champaign.
 *
 * Updated Spring 2021 for WebGL 2.0/GLSL 3.00 ES.
 *
 * You'll need to implement the following functions:
 * setVertex(v, i) - convenient vertex access for 1-D array
 * getVertex(v, i) - convenient vertex access for 1-D array
 * generateTriangles() - generate a flat grid of triangles
 * shapeTerrain() - shape the grid into more interesting terrain
 * calculateNormals() - calculate normals after warping terrain
 *
 * Good luck! Come to office hours if you get stuck!
 */

class Terrain {
    /**
     * Initializes the members of the Terrain object.
     * @param {number} div Number of triangles along the x-axis and y-axis.
     * @param {number} minX Minimum X coordinate value.
     * @param {number} maxX Maximum X coordinate value.
     * @param {number} minY Minimum Y coordinate value.
     * @param {number} maxY Maximum Y coordinate value.
     */
    constructor(div, minX, maxX, minY, maxY) {
        this.div = div;
        this.minX = minX;
        this.minY = minY;
        this.maxX = maxX;
        this.maxY = maxY;

        // Allocate the vertex array
        this.positionData = [];
        // Allocate the normal array.
        this.normalData = [];
        // Allocate the triangle array.
        this.faceData = [];
        // Allocate an array for edges so we can draw a wireframe.
        this.edgeData = [];
        console.log("Terrain: Allocated buffers");

        this.generateTriangles();
        console.log("Terrain: Generated triangles");

        this.generateLines();
        console.log("Terrain: Generated lines");

        this.shapeTerrain();
        console.log("Terrain: Sculpted terrain");

        this.calculateNormals();
        console.log("Terrain: Generated normals");

        // You can use this function for debugging your buffers:
         this.printBuffers();
    }


    //-------------------------------------------------------------------------
    // Vertex access and triangle generation - your code goes here!
    /**
     * Set the x,y,z coords of the ith vertex
     * @param {Object} v An array of length 3 holding the x,y,z coordinates.
     * @param {number} i The index of the vertex to set.
     */
    setVertex(v, i) {
        // MP2: Implement this function!
        this.positionData[i*3] = v[0];
        this.positionData[i*3+1] = v[1];
        this.positionData[i*3+2] = v[2];

    }


    /**
     * Returns the x,y,z coords of the ith vertex.
     * @param {Object} v An array of length 3 to hold the x,y,z coordinates.
     * @param {number} i The index of the vertex to get.
     */
     getVertex(v, i) {
         v[0] = this.positionData[i*3];
         v[1] = this.positionData[i*3 + 1];
         v[2] = this.positionData[i*3 + 2];
         //gotta change the index of the array for both get/set

     }


    /**
     * This function generates the position data(x,y,z) of the vertices, initializing them
     * It also initialized the normal vectors to zeroes and creates the triangles
     */
    generateTriangles() {
        // MP2: Implement the rest of this function!
        var deltaX = (this.maxX - this.minX) / this.div;
        var deltaY = (this.maxY - this.minY) / this.div;


        for(var i = 0; i <= this.div; i++){
          for(var j = 0; j <= this.div; j++){
            this.positionData.push(this.minX+deltaX*j);
            this.positionData.push(this.minY+deltaY*i);
            this.positionData.push(0);
            //top initializes the vertices and bottom the normal arrays
            this.normalData.push(0);
            this.normalData.push(0);
            this.normalData.push(0);
            //missing normal vector
          }
        }

        for(var i = 0; i < this.div; i++){
          for(var j = 0; j < this.div; j++){
            this.faceData.push(j + (this.div+1)*i);
            this.faceData.push(j + (this.div+1)*i + 1);
            this.faceData.push(j + (this.div+1)*i + 1 + this.div);
            //creates the left(first) triangle of the rectangle^
            //second block creates the right(second) triangle of the rectangle
            this.faceData.push(j + (this.div+1)*i + 1);
            this.faceData.push(j + (this.div+1)*i + 2 + this.div);
            this.faceData.push(j + (this.div+1)*i + 1 + this.div);
          }
        }

        //do something similar for faceData


        // We'll need these to set up the WebGL buffers.
        this.numVertices = this.positionData.length/3;
        this.numFaces = this.faceData.length/3;
        this.calculateNormals();
    }


    /**
     * Depending on what part of the terrain it is(also randomness), will
     * increase and decrease the Z value of vertices depending on various factors.
     */
    shapeTerrain() {//set 50 as parameter and .005
//      var randVector = (Math.ceil(Math.random() * (this.maxX-this.minX)),
//                      Math.ceil(Math.random() * (this.maxY-this.minY)),0);
      //might be brackets instead of () for vector
      var delta = 0.005;
      //var vec = glMatrix.vec2.create();
      //vec = glMatrix.vec2.random(vec);
      // have random point using random angle, also have random vec2 that i can use
      // in case the angle one doesnt work
      for(var j = 0; j <= 100; j++){
        //var v = [];
        //var randAngle = Math.random();
      //  randAngle *= 2*Math.PI;
        //var randVector = [Math.cos(randAngle),Math.sin(randAngle),0];
        var randAngle = Math.random();
        randAngle *= 2*Math.PI;
        var randVector = [Math.cos(randAngle),Math.sin(randAngle),0];

        var randX = Math.random();
        var randY = Math.random();
        var sizeX = this.maxX - this.minX;
        var sizeY = this.maxY - this.minY;
        var randPoint = [randX*sizeX+this.minX, randY*sizeY+this.minY, 0];

        for(var i = 0; i <= Math.pow(this.div,2); i++){ //maybe this.div+1
          //check if the this.div^2 was the issue when its fixed

          //this.getVertex(v,i); check if this was issue as well
          var v = [];
          this.getVertex(v,i);
          var temp = [];
          temp[0] = v[0] - randPoint[0];
          temp[1] = v[1] - randPoint[1];
          //var dot = Math.dot(temp,randVector);
          var dot = 0;
          for(var q = 0; q < 2; q++){
            dot += temp[q] * randVector[q];
          }

          if (dot>=0){
            v[2] += delta;
            this.setVertex(v,i);
          }
          else{
            v[2] -= delta;
            this.setVertex(v,i);
          }
        }
      }
      /** randAngle = Math.random();
      randAngle *= 2*Math.PI;
      randVector = [Math.cos(randAngle),Math.sin(randAngle),0];
      randX = Math.random();
      randY = Math.random();
      randPoint = randPoint = [randX*sizeX, randY*sizeY, 0];*/
      //try doin the optional part to get better delta stuffs.
        // MP2: Implement this function!
      //look at the gl.drawelements in terrain.js tab
      //try creating vectors with vec3 instead of using brackets
    }


    /**
     * This function calculates the normals of each face, and then
     * adds them to the vectors themselves.
     */
    calculateNormals() {

      for(var i = 0; i < this.numFaces; i++){
        var face = [];
        for(var k=0;k<3;k++){
          face[k] = this.faceData[3*i+k];
        }
        //var firstVertex = this.faceData[3*i];
        //var secondVertex = this.faceData[3*i+1];
        //var thirdVertes = this.faceData[3*i+2];
        var vertices = [];
        for(var j=0;j<3;j++){
          vertices[j] = [this.positionData[face[j]*3],
          this.positionData[face[j]*3+1],this.positionData[face[j]*3+2]];
          //this for loop gets the three vertices in a triangle for later work
        }

        var normals = [];
        //this for loop accesses the normal vectors that are in place at the moment
        for(var j=0;j<3;j++){
          normals[j] = [this.normalData[face[j]*3],
          this.normalData[face[j]*3+1],this.normalData[face[j]*3+2]];
        }
        var cross1 = [];
        var cross2 = [];
        for(var j=0;j<3;j++){
          cross1[j] = vertices[1][j] - vertices[0][j];
          cross2[j] = vertices[2][j] - vertices[0][j];
        }//subtracting 2-1, 3-1 vertices of the triangle to cross product


        var faceNorm = glMatrix.vec3.create();
        //var sub2 = glMatrix.vec3.create();
        glMatrix.vec3.cross(faceNorm,cross1,cross2);
        //cross product
        for(var j=0;j<3;j++){
          normals[0][j] += faceNorm[j];
          normals[1][j] += faceNorm[j];
          normals[2][j] += faceNorm[j];
        }
        for(var j=0;j<3;j++){
          [this.normalData[face[j]*3], this.normalData[face[j]*3+1],
          this.normalData[face[j]*3+2]] = normals[j];

        }

        //cross product of two vectors of a->b a->c
        //add to normals of the three vertices in normaldata
      }

      for(var i = 0; i <= this.numVertices; i++){
        //normalize each vertex normal
        var vertex = [this.normalData[i*3], this.normalData[i*3+1], this.normalData[i*3+2]];
        glMatrix.vec3.normalize(vertex,vertex);//add another vec3 in case
        [this.normalData[i*3], this.normalData[i*3+1], this.normalData[i*3+2]] = vertex;
      }

        // MP2: Implement this function!

    }


    //-------------------------------------------------------------------------
    // Setup code (run once)
    /**
     * Generates line data from the faces in faceData for wireframe rendering.
     */
    generateLines() {
        for (var f = 0; f < this.faceData.length/3; f++) {
            // Calculate index of the face
            var fid = f*3;
            this.edgeData.push(this.faceData[fid]);
            this.edgeData.push(this.faceData[fid+1]);

            this.edgeData.push(this.faceData[fid+1]);
            this.edgeData.push(this.faceData[fid+2]);

            this.edgeData.push(this.faceData[fid+2]);
            this.edgeData.push(this.faceData[fid]);
        }
    }


    /**
     * Sets up the WebGL buffers and vertex array object.
     * @param {object} shaderProgram The shader program to link the buffers to.
     */
    setupBuffers(shaderProgram) {
        // Create and bind the vertex array object.
        this.vertexArrayObject = gl.createVertexArray();
        gl.bindVertexArray(this.vertexArrayObject);

        // Create the position buffer and load it with the position data.
        this.vertexPositionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexPositionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this.positionData),
                      gl.STATIC_DRAW);
        this.vertexPositionBuffer.itemSize = 3;
        this.vertexPositionBuffer.numItems = this.numVertices;
        console.log("Loaded ", this.vertexPositionBuffer.numItems, " vertices.");

        // Link the position buffer to the attribute in the shader program.
        gl.vertexAttribPointer(shaderProgram.locations.vertexPosition,
                               this.vertexPositionBuffer.itemSize, gl.FLOAT,
                               false, 0, 0);
        gl.enableVertexAttribArray(shaderProgram.locations.vertexPosition);

        // Specify normals to be able to do lighting calculations
        this.vertexNormalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexNormalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this.normalData),
                      gl.STATIC_DRAW);
        this.vertexNormalBuffer.itemSize = 3;
        this.vertexNormalBuffer.numItems = this.numVertices;
        console.log("Loaded ", this.vertexNormalBuffer.numItems, " normals.");

        // Link the normal buffer to the attribute in the shader program.
        gl.vertexAttribPointer(shaderProgram.locations.vertexNormal,
                               this.vertexNormalBuffer.itemSize, gl.FLOAT,
                               false, 0, 0);
        gl.enableVertexAttribArray(shaderProgram.locations.vertexNormal);

        // Set up the buffer of indices that tells WebGL which vertices are
        // part of which triangles.
        this.triangleIndexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.triangleIndexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(this.faceData),
                      gl.STATIC_DRAW);
        this.triangleIndexBuffer.itemSize = 1;
        this.triangleIndexBuffer.numItems = this.faceData.length;
        console.log("Loaded ", this.triangleIndexBuffer.numItems, " triangles.");

        // Set up the index buffer for drawing edges.
        this.edgeIndexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.edgeIndexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(this.edgeData),
                      gl.STATIC_DRAW);
        this.edgeIndexBuffer.itemSize = 1;
        this.edgeIndexBuffer.numItems = this.edgeData.length;

        // Unbind everything; we want to bind the correct element buffer and
        // VAO when we want to draw stuff
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.bindVertexArray(null);
    }


    //-------------------------------------------------------------------------
    // Rendering functions (run every frame in draw())
    /**
     * Renders the terrain to the screen as triangles.
     */
    drawTriangles() {
        gl.bindVertexArray(this.vertexArrayObject);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.triangleIndexBuffer);
        gl.drawElements(gl.TRIANGLES, this.triangleIndexBuffer.numItems,
                        gl.UNSIGNED_INT,0);
    }


    /**
     * Renders the terrain to the screen as edges, wireframe style.
     */
    drawEdges() {
        gl.bindVertexArray(this.vertexArrayObject);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.edgeIndexBuffer);
        gl.drawElements(gl.LINES, this.edgeIndexBuffer.numItems,
                        gl.UNSIGNED_INT,0);
    }
    getHighAndLow(){
      var high = -100000;
      var low = 100000;
      for (var i=0;i<this.numVertices;i++){
        var zValue = this.positionData[3*i+2];
        if(zValue>high){
          high = zValue;
        }
        if(zValue<low){
          low = zValue;
        }
      }
      return high,low;
    }

    //-------------------------------------------------------------------------
    // Debugging
    /**
     * Prints the contents of the buffers to the console for debugging.
     */
    printBuffers() {
        for (var i = 0; i < this.numVertices; i++) {
            console.log("v ", this.positionData[i*3], " ",
                              this.positionData[i*3 + 1], " ",
                              this.positionData[i*3 + 2], " ");
        }
        for (var i = 0; i < this.numVertices; i++) {
            console.log("n ", this.normalData[i*3], " ",
                              this.normalData[i*3 + 1], " ",
                              this.normalData[i*3 + 2], " ");
        }
        for (var i = 0; i < this.numFaces; i++) {
            console.log("f ", this.faceData[i*3], " ",
                              this.faceData[i*3 + 1], " ",
                              this.faceData[i*3 + 2], " ");
        }
    }

} // class Terrain
