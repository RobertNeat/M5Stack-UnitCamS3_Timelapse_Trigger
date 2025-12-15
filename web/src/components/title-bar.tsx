// "use client";

import { useNavigate, useLocation } from "react-router-dom";
import { Button } from "@nextui-org/react";
import '../styles/title-bar.css';

export default function TitleBar() {
  const navigate = useNavigate();
  const location = useLocation();
  const isHomePage = location.pathname === "/";

  const handleBackClick = () => {
    navigate(-1);
  };

  return (
    <div className="title-bar fixed w-full h-16 z-50 flex">
      <div className="w-full ml-5 mt-1.5 flex flex-col">
        <div className="flex items-center">
          {!isHomePage && (
            <Button
              isIconOnly
              variant="light"
              size="sm"
              onClick={handleBackClick}
              className="back-button mr-3"
              aria-label="Go back"
            >
              <svg
                xmlns="http://www.w3.org/2000/svg"
                fill="none"
                viewBox="0 0 24 24"
                strokeWidth={1.5}
                stroke="currentColor"
                className="w-5 h-5"
              >
                <path strokeLinecap="round" strokeLinejoin="round" d="M10.5 19.5L3 12m0 0l7.5-7.5M3 12h18" />
              </svg>
            </Button>
          )}
          <p className="title-text text-xl">Unit_Cam_S3 Timelapse trigger</p>
        </div>

        <div className="flex ml-px mr-5">
          <p className="subtitle-text">User Demo</p>

          <div className="grow"></div>

          {/* <Link
            href={docLink}
            underline="hover"
            color="foreground"
            className="mr-5 font-serif font-light"
          >
            Document
          </Link>

          <Link
            href={githubLink}
            underline="hover"
            color="foreground"
            className="font-serif font-light"
          >
            Github
          </Link> */}

        </div>
      </div>
    </div>
  );
}
