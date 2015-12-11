#pragma once

class VView;

class VCoreApi VViewManager : public VAllocator
{
public:
	VViewManager();
	~VViewManager();

	static VViewManager* GetInstance();
	std::vector<VView*>& getViews() { return mViews; }

private:
	std::vector<VView*> mViews;
	static VViewManager mInstance;
};

