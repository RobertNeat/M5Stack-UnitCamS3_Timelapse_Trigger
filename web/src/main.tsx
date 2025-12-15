import React from "react";
import ReactDOM from "react-dom/client";
import { createBrowserRouter, RouterProvider } from "react-router-dom";
import { NextUIProvider } from "@nextui-org/react";
import "./index.css";
import ErrorPage from "./error-page";
import PageRoot from "./routes/page-root";
import PageShooter from "./routes/page-shooter";
import PageShooterStart from "./routes/page-shooter-start";

const router = createBrowserRouter([
  {
    path: "/",
    element: (
      <NextUIProvider>
        <PageRoot></PageRoot>
      </NextUIProvider>
    ),
    errorElement: <ErrorPage></ErrorPage>,
  },
  {
    path: "/shooter",
    element: (
      <NextUIProvider>
        <PageShooter></PageShooter>
      </NextUIProvider>
    ),
  },
  {
    path: "/shooter_start",
    element: (
      <NextUIProvider>
        <PageShooterStart></PageShooterStart>
      </NextUIProvider>
    ),
  },
]);

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <RouterProvider router={router}></RouterProvider>
  </React.StrictMode>
);
