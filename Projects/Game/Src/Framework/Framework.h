#pragma once

class Framework {
public:
	Framework() = default;
	Framework(const Framework&) = delete;
	Framework(Framework&&) = delete;
	virtual ~Framework() = default;

	Framework& operator=(const Framework&) = delete;
	Framework& operator=(Framework&&) = delete;

protected:
	virtual void Initialize();

	virtual void Finalize();

	virtual void Update() = 0;

	virtual void Draw() = 0;

public:
	void Execution();

protected:
	bool isEnd_;
};