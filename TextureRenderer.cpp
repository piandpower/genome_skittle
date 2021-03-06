#include "TextureRenderer.h"
#include "Interpreter.h"
#include "Color.h"
#include <QtOpenGL>
#include <stdlib.h>

using namespace std;
using namespace skittle;

/**
 * Renderer should setup a graph based on a colorlist and some other 
 * details.  We're kindof going to assume everyone else has either done 
 * their job or can live with the consequences.
 * 
 **/

TextureRenderer::TextureRenderer(Interpreter* abstractGraph) :
    Renderer::Renderer(abstractGraph) {
    enableTextures_ = getMaxTextureSize() > 0;
}

bool TextureRenderer::refresh(){
    
    int max_size = getMaxTextureSize();
    
    //pad the end with white pixels, background color //or lets don't until we understand why.
    //~ for(int i = 0; i <= width; ++i)
        //~ pixels.push_back( Color(128,128,128) );

    width_ = current_->getWidth();
    height_ = current_->getPx()->size() / width_;

    if(enableTextures_)
    {
        //determine the size of the texture canvas
        int canvas_width = width_ / max_size + 1; //AKA ciel();
        canvas_width = min((maxSaneWidth / max_size + 1),canvas_width);

        int canvas_height = height_ / max_size + 1;
        createEmptyTiles(canvas_width, canvas_height);

        const vector<Color>* pixels = current_->getPx();
        for(vector<Color>::size_type i = 0; i < pixels->size(); i++)
        {
            //this is specifically using the 'operator [] (int)' method defined by Interpreter.
            Color c1 = current_->getPxAt(i);

            tile_matrix::size_type x = (i % width_) / max_size; //(horizontal Index)
            tile_matrix::size_type y = (i / width_) / max_size; //(vertical Index)

            if (x >= canvas_.size() || y >= canvas_.size()) continue;

            vector<unsigned char>& current = canvas_[x][y].data_;
            current.push_back(c1.getRed());
            current.push_back(c1.getGreen());
            current.push_back(c1.getBlue());
        }
        
        for(unsigned int x = 0; x < canvas_.size(); ++x)
        {
            for(unsigned int y = 0; y < canvas_[x].size(); ++y)
            {
                loadTexture(canvas_[x][y]);//tex_ids.push_back(
                canvas_[x][y].data_.clear();//the data has been loaded into the graphics card
            }
        }
    }
    return true;
}

void TextureRenderer::createEmptyTiles(int canvas_width, int canvas_height)
{
    int max_size = getMaxTextureSize();

    //load pixels to canvases;
    for(int x = 0; x < canvas_width ; ++x) {
        canvas_.push_back(vector<Tile>());
        for(int y = 0; y < canvas_height; ++y) {
            Tile t;
            t.width_ = min(width_ - (x * max_size), max_size);
            t.height_ = min(height_ - (y * max_size), max_size);
            canvas_[x].push_back(t);
        }
    }
}

TextureRenderer::~TextureRenderer()
{
    for(unsigned int x = 0; x < canvas_.size(); ++x)
        for(unsigned int y = 0; y < canvas_[x].size(); ++y)
            canvas_[x][y].data_.clear();//the data has been loaded into the graphics card
}

GLuint TextureRenderer::loadTexture(Tile& tile)
{
	glGenTextures( 1, &tile.tex_id );//TODO: could we store this in tile?
    glBindTexture (GL_TEXTURE_2D, tile.tex_id);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_NEAREST
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);               
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, tile.width_, tile.height_, 0, GL_RGB, GL_UNSIGNED_BYTE, &tile.data_[0]);
    
    return tile.tex_id;
}

int TextureRenderer::getMaxTextureSize(){
    static int max_size = 0;
    if (0 == max_size){        
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
        max_size = min(0,max_size);
    }
    return max_size;
}

int TextureRenderer::draw()//draws from canvas
{	int render_width = 0;
	int max_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
	glPushMatrix();
	for(unsigned int x =0; x < canvas_.size(); ++x)
	{
		for(unsigned int y = 0; y < canvas_[x].size(); ++y)
		{	
			Tile& tile = canvas_[x][y];
			glPushMatrix();
			glTranslated(x * max_size, y * max_size, 0);
			glColor3d(1.0,1.0,1.0);
		    glEnable (GL_TEXTURE_2D); /* enable texture mapping */
		    glBindTexture (GL_TEXTURE_2D, tile.tex_id); /* bind to our texture, has id of 13 */
			
		    glBegin (GL_QUADS);
		        glTexCoord2f (0.0f,0.0f); /* upper left corner of image */
		        glVertex3f (0.0f, 0.0f, 0.0f);
		        glTexCoord2f (1.0f, 0.0f); /* upper right corner of image */
		        glVertex3f (tile.width_, 0.0f, 0.0f);
		        glTexCoord2f (1.0f, 1.0f); /* lower right corner of image */
		        glVertex3f (tile.width_, tile.height_, 0.0f);
		        glTexCoord2f (0.0f, 1.0f); /* lower left corner of image */
		        glVertex3f (0.0f, tile.height_, 0.0f);
		    glEnd ();
		
		    glDisable (GL_TEXTURE_2D); /* disable texture mapping */
			glPopMatrix();
			
			render_width += tile.width_;
		}
	}	
	glPopMatrix();
	return render_width;
}