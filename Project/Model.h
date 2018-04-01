#ifndef _CG_MODEL_H_
#define _CG_MODEL_H_

#include "DisallowCopy.h"
#include "ModelPart.h"
#include <vector>
#include <string>
#include "CGTexture.h"

// The SGF model class
class Model : private DisallowCopy
{
public:
	Model(void);
	virtual ~Model(void);
	// Draw the model
	void draw(void);

	// Load a SGF file, can throw an Error class
	void loadSGF(const char *pFilename);
	void clear(void);

	// Set the texture for the model, a box mapping is done with the given bounding box
	void setTexture(CGTexture *pTexture, const BoundingBox &box);

	// Exception class
	class Error
	{
	public:
		Error(const char *pMsg) { _msg = pMsg; }
		Error(const std::string msg) { _msg = msg; }
		std::string what() { return _msg; }
	private:
		std::string _msg;
	};
private:
	std::vector<ModelPart*> _modelPartPtrs;
	CGTexture				*_pTexture;
};

struct VertexTypeNameMapping
{
	GLenum			glType;
	const char		*pName;
};


#endif