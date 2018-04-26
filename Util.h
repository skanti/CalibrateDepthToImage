#pragma once


inline static void load_extrinsics_intrinsics(std::string filename, Eigen::Matrix4f &intr, Eigen::Matrix4f &extr) {

	std::ifstream file (filename);
	std::string line;
	if (file.is_open()) {
		std::getline (file, line);
		std::getline (file, line);
		std::getline (file, line);
		std::getline (file, line);

		intr.setIdentity();
		extr.setIdentity();

		int width = 0;
		int height = 0;
		std::string dummy;

		{
			std::stringstream ss(line);
			ss >> dummy;
			ss >> width;
		}

		{
			std::getline (file, line);
			std::stringstream ss(line);
			ss >> dummy;
			ss >> height;
		}

		auto fchars = [](char c) {
			switch(c) {
				case '[':
				case ']':
				case ',':
				case ':':
					return true;
				default:
					return false;
			}
		};

		{
			std::getline (file, line);
			line.erase(std::remove_if(line.begin(), line.end(), fchars), line.end());
			std::stringstream ss(line);
			ss >> dummy;
			ss >> intr(0, 0);
			ss >> intr(1, 1);
			ss >> intr(0, 2);
			ss >> intr(1, 2);
		}

		//intr.col(2) = -intr.col(2);


		Eigen::Quaternionf q;
		Eigen::Matrix3f R;
		{
			std::getline (file, line); std::getline (file, line); std::getline (file, line);
			line.erase(std::remove_if(line.begin(), line.end(), fchars), line.end());
			std::stringstream ss(line);
			ss >> dummy;
			ss >> q.x();
			ss >> q.y();
			ss >> q.z();
			ss >> q.w(); 
		}
		R = q.normalized().toRotationMatrix();

		Eigen::Vector3f t;
		{
			std::getline (file, line);
			line.erase(std::remove_if(line.begin(), line.end(), fchars), line.end());
			std::stringstream ss(line);
			ss >> dummy;
			ss >> t(0);
			ss >> t(1);
			ss >> t(2);
		}

		extr.block(0, 0, 3, 3) = R;
		extr.block(0, 3, 3, 1)= t;

		file.close();
	}       

}
